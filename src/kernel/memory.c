#include <stdint.h>
#include <stddef.h>
#include "drivers/uart.h"
#include "memory.h"

#define PAGE_SIZE 4096

struct Page {
	struct Page *next;
};

static struct Page *free_list;

extern uint8_t _bss_end[]; // named in the linker script

void page_init() {
	kprint("Initialising page allocator.\n");
	uintptr_t start = ((uintptr_t)_bss_end + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
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