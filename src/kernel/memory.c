#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "drivers/uart.h"
#include "memory.h"
#include "lib/string.h"
#include "interrupts.h"

static struct Page *free_list = NULL;

HeapHeader *heap_free_list;

// named in the linker script
extern uint8_t _heap_start[];
extern uint8_t _bss_start[];
extern uint8_t _bss_end[];

uintptr_t heap_start;
const uintptr_t heap_end = 0x88000000; // Default QEMU RAM limit

void zero_bss()
{
	memset(_bss_start, 0, (size_t)_bss_end - (size_t)_bss_start);
}

void page_init()
{
	kprint("Initialising page allocator...");

	heap_start = align_up((size_t)_heap_start, PAGE_SIZE);

	for (uintptr_t addr = heap_start; addr + PAGE_SIZE <= heap_end; addr += PAGE_SIZE)
	{
		page_free((void *)addr);
	}
	kputs("OK");

	test_memory_integrity();
	test_memory_alignment();
	test_memory_stress();
	test_memory_reuse();
	// page_free((void *)23);
}

int is_aligned_to(size_t value, size_t size)
{
	return value % size == 0;
}

size_t align_up(size_t value, size_t size)
{
	return (value + size - 1) & ~(size - 1);
}

size_t align_down(size_t value, size_t size)
{
	return value & ~(size - 1);
}

void page_free(void *addr){
	if (addr == NULL)
		return;
	if (!is_aligned_to((size_t)addr, PAGE_SIZE))
		kpanic("page_free: address not aligned to PAGE_SIZE: %p", addr);

	struct Page *p = (struct Page *)addr;
	p->next = free_list;
	free_list = p;
}

HeapHeader *page_alloc()
{
	if (free_list == NULL)
	{
		kpanic("No free pages!");
	}
	struct Page *p = free_list;
	free_list = free_list->next;

	// zero out the page
	memset(p, 0, PAGE_SIZE);

	HeapHeader *header = (HeapHeader *)p;

	header->size = PAGE_SIZE - sizeof(HeapHeader);
	header->is_free = 1;
	header->next = NULL;
	header->prev = NULL;

	return header;
}

void *kmalloc(size_t size)
{
	if (size == 0)
		return NULL;

	size = align_up(size, 16);

	if (heap_free_list == NULL)
	{
		HeapHeader *header = (HeapHeader *)page_alloc();
		kheap_insert_sorted(header);
	}

	HeapHeader *current = heap_free_list;

	while (current != NULL)
	{
		if (current->is_free && current->size >= size)
		{
			break;
		}
		current = current->next;
	}

	if (current == NULL)
	{
		HeapHeader *header = (HeapHeader *)page_alloc();
		kheap_insert_sorted(header);
		current = header;
	}

	if (!current)
	{
		return NULL;
	}

	current->is_free = 0;

	kheap_split(current, size);

	return (void *)((char *)current + sizeof(HeapHeader));
}

void kfree(void *ptr)
{
	if (ptr == NULL)
		return;

	HeapHeader *header = (HeapHeader *)((uint8_t *)ptr - sizeof(HeapHeader));
	if (header->is_free)
	{
		kpanic("Double free detected!");
	}
	header->is_free = 1;

	memset(ptr,0,header->size);

	HeapHeader *final_block = kcoalesce(header);
	uintptr_t block_start = (uintptr_t)final_block;
	uintptr_t block_end = block_start + sizeof(HeapHeader) + final_block->size;
	uintptr_t first_page = align_up((uintptr_t)block_start + sizeof(HeapHeader), PAGE_SIZE);
	uintptr_t last_page = align_down((uintptr_t)block_end, PAGE_SIZE);

	if (first_page < last_page)
	{
		if (block_start < first_page)
		{
			kheap_split(final_block, first_page - block_start - sizeof(HeapHeader));
			final_block = final_block->next;
		}

		size_t data_to_free = last_page - (uintptr_t)final_block - sizeof(HeapHeader);

		kheap_split(final_block, data_to_free);

		if (final_block->prev)
			final_block->prev->next = final_block->next;
		if (final_block->next)
			final_block->next->prev = final_block->prev;
		if (final_block == heap_free_list)
			heap_free_list = final_block->next;

		for (uintptr_t addr = first_page; addr < last_page; addr += PAGE_SIZE)
		{
			page_free((void *)addr);
		}

	}
	else
	{
		if (final_block->prev == NULL && final_block->next == NULL && final_block != heap_free_list)
		{
			kheap_insert_sorted(final_block);
		}
	}
}

HeapHeader *kcoalesce(HeapHeader *header)
{
	if (!header || !header->is_free)
		return header;

	// walk back
	while (header->prev && header->prev->is_free)
	{
		uintptr_t prev_end = (uintptr_t)header->prev + sizeof(HeapHeader) + header->prev->size;
		if (prev_end == (uintptr_t)header)
		{
			header = header->prev;
		}
		else
		{
			break;
		}
	}

	// merge forward
	while (header->next && header->next->is_free)
	{
		uintptr_t current_end = (uintptr_t)header + sizeof(HeapHeader) + header->size;
		if (current_end == (uintptr_t)header->next)
		{
			header->size += sizeof(HeapHeader) + header->next->size;
			header->next = header->next->next;
			if (header->next)
			{
				header->next->prev = header;
			}
		}
		else
		{
			break;
		}
	}
	return header;
}

void kheap_split(HeapHeader *header, size_t size)
{
	size = align_up(size, 16);
	size_t min_split_size = sizeof(HeapHeader) + MIN_HEAP_CHUNK_SIZE;

	if (header->size >= size + min_split_size)
	{
		HeapHeader *new_header = (HeapHeader *)((uint8_t *)header + sizeof(HeapHeader) + size);
		if ((uintptr_t)new_header < heap_start || (uintptr_t)new_header > heap_end)
		{
			kpanic("Splitting created invalid pointer: %x!", (uint64_t)new_header);
		}
		new_header->size = header->size - size - sizeof(HeapHeader);
		new_header->is_free = 1;

		new_header->next = header->next;
		new_header->prev = header;
		if (header->next)
		{
			header->next->prev = new_header;
		}
		header->size = size;
		header->next = new_header;
	}
}

void kheap_insert_sorted(HeapHeader *new_block)
{
	if (heap_free_list == NULL || (uintptr_t)new_block < (uintptr_t)heap_free_list)
	{
		new_block->next = heap_free_list;
		if (heap_free_list)
			heap_free_list->prev = new_block;
		heap_free_list = new_block;
		new_block->prev = NULL;
		return;
	}

	HeapHeader *curr = heap_free_list;
	while (curr->next != NULL && (uintptr_t)curr->next < (uintptr_t)new_block)
	{
		curr = curr->next;
	}

	new_block->next = curr->next;
	new_block->prev = curr;
	if (curr->next)
		curr->next->prev = new_block;
	curr->next = new_block;
}

void test_memory_integrity()
{
	kprint("Running Integrity Test...\n");
	uint64_t *a = (uint64_t *)kmalloc(16);
	uint64_t *b = (uint64_t *)kmalloc(16);
	uint64_t *c = (uint64_t *)kmalloc(512);
	uint64_t *d = (uint64_t *)kmalloc(2048);
	uint64_t *e = (uint64_t *)kmalloc(1024);
	uint64_t *f = (uint64_t *)kmalloc(2048);

	*a = 0x1122334455667788;
	*b = 0x99AABBCCDDEEFF00;

	heap_stats();

	if (*a == 0x1122334455667788)
	{
		kprint("Integrity Pass!\n");
	}
	else
	{
		kprint("CORRUPTION DETECTED!\n");
	}
	kfree(a);
	kfree(f);
	heap_stats();
	kfree(b);
	kfree(c);
	kfree(d);
	kfree(e);
}

void test_memory_reuse()
{
    kprint("Running Reuse Test...\n");

    uint8_t *big = (uint8_t *)kmalloc(256);
    if (!big) {
        kprint("Allocation failed – aborting reuse test.\n");
        return;
    }

    for (size_t i = 0; i < 256; ++i) {
        big[i] = 0xAA;
    }

    kfree(big);
    heap_stats();

    uint8_t *small = (uint8_t *)kmalloc(32);
    if (!small) {
        kprint("Second allocation failed – aborting reuse test.\n");
        return;
    }

    bool dirty = false;
    for (size_t i = 0; i < 32; ++i) {
        if (small[i] != 0) {
			kprintf("bad value %x", small[i]);
            dirty = true;
            break;
        }
    }

    if (dirty) {
        kprint("FAIL: Reused block contains leftover data!\n");
    } else {
        kprint("PASS: Reused block is clean.\n");
    }

    kfree(small);
    heap_stats();
}

void test_memory_alignment()
{
	kprint("Running Alignment Test...\n");
	for (int i = 1; i <= 64; i++)
	{
		void *ptr = kmalloc(i);
		if (((uintptr_t)ptr % 8) != 0)
		{
			kprint("Misaligned allocation detected!\n");
			return;
		}
		kfree(ptr);
	}
	kprint("All allocations are properly aligned!\n");
}

void test_memory_stress()
{
	kprint("\nPre-stress heap stats:\n");
	heap_stats();
	kprintf("Starting Stress Test...\n");
	void *ptrs[100] = {0};
	uint32_t seed = 0xACE2026;

	for (int i = 0; i < 1000; i++)
	{
		int idx = (seed >> 16) % 50;
		if (ptrs[idx] == NULL)
		{
			size_t size = (seed % 256) + 1;
			ptrs[idx] = kmalloc(size);
		}
		else
		{
			kfree(ptrs[idx]);
			ptrs[idx] = NULL;
		}
		// Simple LCG to "randomize" seed
		seed = (seed * 1103515245 + 12345) & 0x7fffffff;
	}
	kprintf("Stress Test Finished. Check heap_stats() for sanity.\n");
	kprintf("HeadHeader size: %d bytes\n", sizeof(HeapHeader));
	heap_stats();
	kprintf("Cleaning up remaining allocations...\n");
	for (int i = 0; i < 100; i++)
	{
		if (ptrs[i] != NULL)
		{
			kfree(ptrs[i]);
			ptrs[i] = NULL;
		}
	}
	heap_stats();
}

void heap_stats()
{
	size_t free_size = 0;
	size_t used_size = 0;
	size_t free_blocks = 0;
	size_t used_blocks = 0;

	HeapHeader *current = heap_free_list;

	while (current != NULL)
	{
		if (current->is_free)
		{
			free_size += current->size;
			free_blocks++;
		}
		else
		{
			used_size += current->size;
			used_blocks++;
		}
		current = current->next;
	}

	kprint("--- Kernel Heap Stats ---\n");
	kprintf("Page Size: %d bytes", PAGE_SIZE);
	kprintf("HeapHeader size: %d bytes", sizeof(HeapHeader));
	kprintf("Pages allocated: %d", (used_size + free_size + used_blocks * sizeof(HeapHeader) + free_blocks * sizeof(HeapHeader)) / PAGE_SIZE);
	kprintf("Used: %x bytes in %d blocks", used_size, used_blocks);
	kprintf("Used (with overhead): %x bytes", used_size + used_blocks * sizeof(HeapHeader));
	kprintf("Free: %x bytes in %d blocks", free_size, free_blocks);
	kprintf("Total metadata overhead: %d bytes", ((used_blocks + free_blocks) * sizeof(HeapHeader)));
	kprint("-------------------------\n");
}