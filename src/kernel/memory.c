#include <stdint.h>
#include <stddef.h>
#include "drivers/uart.h"
#include "memory.h"
#include "panic.h"

#define PAGE_SIZE 4096

struct Page {
	struct Page *next;
};

struct HeapHeader {
	size_t size;
	int is_free;
	struct HeapHeader *next;
};

static struct Page *free_list = NULL;

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

void *page_alloc() { {
	if (free_list == NULL) {
		kpanic("No free pages!");
	}
	struct Page *p = free_list;
	free_list = free_list->next;

	//zero out the page
	for (int i = 0; i < (PAGE_SIZE/8); i++) {
		((uint8_t *)p)[i] = 0;
	}

	return (void *)p;
}
}