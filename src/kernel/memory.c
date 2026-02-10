#include <stdint.h>
#include <stddef.h>
#include "drivers/uart.h"
#include "memory.h"
#include "interrupts.h"

static struct Page *free_list = NULL;

HeapHeader *heap_free_list;
extern uint8_t _heap_start[]; // named in the linker script

void page_init()
{
	kprint("Initialising page allocator...");
	uintptr_t start = ((uintptr_t)_heap_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	uintptr_t end = 0x88000000; // Default QEMU RAM limit

	for (uintptr_t addr = start; addr + PAGE_SIZE <= end; addr += PAGE_SIZE)
	{
		page_free((void *)addr);
	}
	kputs("OK");
}

void page_free(void *addr)
{
	if (addr == NULL)
		return;

	struct Page *p = (struct Page *)addr;
	p->next = free_list;
	free_list = p;
}

void *page_alloc()
{
	if (free_list == NULL)
	{
		kpanic("No free pages!");
	}
	struct Page *p = free_list;
	free_list = free_list->next;

	// zero out the page
	for (int i = 0; i < (PAGE_SIZE); i++)
	{
		((uint8_t *)p)[i] = 0;
	}

	return (void *)p;
}

void *kmalloc(size_t size)
{
	if (size == 0)
		return NULL;

	// Align size to 8 bytes
	size = (size + 7) & ~7;

	if (heap_free_list == NULL)
	{
		HeapHeader *header = (HeapHeader *)page_alloc();
		header->size = PAGE_SIZE - sizeof(HeapHeader);
		header->is_free = 1;
		header->next = NULL;
		header->prev = NULL;

		heap_free_list = header;
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
		header->size = PAGE_SIZE - sizeof(HeapHeader);
		header->is_free = 1;
		header->next = heap_free_list;
		header->prev = NULL;
		if (heap_free_list)
		{
			heap_free_list->prev = header;
		}
		heap_free_list = header;
		current = header;
	}

	if (!current)
	{
		return NULL;
	}

	current->is_free = 0;

	size_t min_split_size = sizeof(HeapHeader) + 16;
	if (current->size >= size + min_split_size)
	{
		HeapHeader *new_header = (HeapHeader *)((uint8_t *)current + sizeof(HeapHeader) + size);
		if ((uintptr_t)new_header < 0x80000000 || (uintptr_t)new_header > 0x88000000)
		{
			kpanic("Splitting created invalid pointer!");
		}
		new_header->size = current->size - size - sizeof(HeapHeader);
		new_header->is_free = 1;

		new_header->next = current->next;
		new_header->prev = current;
		if (current->next)
		{
			current->next->prev = new_header;
		}
		current->size = size;
		current->next = new_header;
	}

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

	kcoalesce(header);
}

void kcoalesce(HeapHeader *header)
{
	if (!header || !header->is_free)
		return;
	// jump backward
	while (header->prev && header->prev->is_free)
	{
		header = header->prev;
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
}

void test_memory_integrity()
{
	kprint("Running Integrity Test...\n");
	uint64_t *a = (uint64_t *)kmalloc(16);
	uint64_t *b = (uint64_t *)kmalloc(16);

	*a = 0x1122334455667788;
	*b = 0x99AABBCCDDEEFF00;

	if (*a == 0x1122334455667788)
	{
		kprint("Integrity Pass!\n");
	}
	else
	{
		kprint("CORRUPTION DETECTED!\n");
	}
	kfree(a);
	kfree(b);
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