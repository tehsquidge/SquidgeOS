#include <stdint.h>
#include <stddef.h>
#include "drivers/uart.h"
#include "memory.h"
#include "panic.h"


HeapHeader* heap_free_list;
extern uint8_t _heap_start[]; // named in the linker script

void page_init() {
	kprint("Initialising page allocator.\n");
	uintptr_t start = ((uintptr_t)_heap_start + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
	uintptr_t end = 0x88000000; // Default QEMU RAM limit

	for (uintptr_t addr = start; addr + PAGE_SIZE <= end; addr += PAGE_SIZE) {
		page_free((void *)addr);
	}
}

void page_free(void *addr) {
	if (addr == NULL) return;

	struct Page *p = (struct Page *)addr;
	p->next = free_list;
	free_list = p;
}

void *page_alloc() {
	if (free_list == NULL) {
		kpanic("No free pages!");
	}
	struct Page *p = free_list;
	free_list = free_list->next;

	//zero out the page
	for (int i = 0; i < (PAGE_SIZE); i++) {
		((uint8_t *)p)[i] = 0;
	}

	return (void *)p;
}

void *kmalloc(size_t size) {
	if (size == 0) return NULL;

	// Align size to 8 bytes
	size = (size + 7) & ~7;

	if (heap_free_list == NULL) {
		HeapHeader* header = (HeapHeader *)page_alloc();
		header->size = PAGE_SIZE - sizeof(HeapHeader);
		header->is_free = 1;
		header->next = NULL;
		
		heap_free_list = header;
	}

	HeapHeader *current = heap_free_list;
	HeapHeader *prev = NULL;

	while (current != NULL) {
		if (current->is_free && current->size >= size) {
			break;
		}
		prev = current;
		current = current->next;
	}

	if(current == NULL) {
		// No suitable block found, allocate a new page
		HeapHeader* header = (HeapHeader *)page_alloc();
		header->size = PAGE_SIZE - sizeof(HeapHeader);
		header->is_free = 1;
		header->next = NULL;

		if (prev) {
			prev->next = header;
		} else {
			heap_free_list = header;
		}
		current = header;
	}

	// Now, current is a block that can be used
    if (!current) {
        return NULL;
    }

	current->is_free = 0;

	// If the block is larger than needed, split it
	size_t min_split_size = sizeof(HeapHeader) + 16;
	if (current->size >= size + min_split_size) {
		HeapHeader *new_header = (HeapHeader *)((uint8_t *)current + sizeof(HeapHeader) + size);
		new_header->size = current->size - size - sizeof(HeapHeader);
		new_header->is_free = 1;
		new_header->next = current->next;

		current->size = size;
		current->next = new_header;
	}

    return (void*)((char*)current + sizeof(HeapHeader));
}

void kfree(void *ptr) {
	if (ptr == NULL) return;

	HeapHeader *header = (HeapHeader *)((uint8_t *)ptr - sizeof(HeapHeader));
	if(header->is_free) {
		kpanic("Double free detected!");
	}
	header->is_free = 1;

	kcoalesce(header);
}

void kcoalesce(HeapHeader *header) {
	if(!header || !header->next || !header->is_free) return;
	uintptr_t current_end = (uintptr_t)header + sizeof(HeapHeader) + header->size;
	if (current_end == (uintptr_t)header->next && header->next->is_free) {
		header->size += sizeof(HeapHeader) + header->next->size;
		header->next = header->next->next;
		kcoalesce(header);
	}
}

void test_memory_integrity() {
    kprint("Running Integrity Test...\n");
    uint64_t *a = (uint64_t*)kmalloc(16);
    uint64_t *b = (uint64_t*)kmalloc(16);
    
    *a = 0x1122334455667788;
    *b = 0x99AABBCCDDEEFF00;

    if (*a == 0x1122334455667788) {
        kprint("Integrity Pass!\n");
    } else {
        kprint("CORRUPTION DETECTED!\n");
    }
    kfree(a);
    kfree(b);
}