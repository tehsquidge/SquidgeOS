#ifndef KMEMORY_H
#define KMEMORY_H

#define PAGE_SIZE 4096

typedef struct Page
{
	struct Page *next;
} Page;

typedef struct HeapHeader
{
	size_t size;
	int is_free;
	struct HeapHeader *next;
	struct HeapHeader *prev;
} HeapHeader;

void zero_bss();
int is_aligned_to(size_t value, size_t size);
size_t align_up(size_t value, size_t size);
size_t align_down(size_t value, size_t size);
void page_init();
void page_free(void *addr);
void *page_alloc();
void *kmalloc(size_t size);
void kfree(void *ptr);
void kcoalesce(HeapHeader *header);
void test_memory_integrity();
void test_memory_alignment();
void test_memory_stress();
void heap_stats();

#endif
