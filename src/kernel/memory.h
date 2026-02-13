#ifndef KMEMORY_H
#define KMEMORY_H

#define PAGE_SIZE 4096
#define MIN_HEAP_CHUNK_SIZE 16U

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
	uint64_t _padding;
} __attribute__((aligned(16))) HeapHeader;

void zero_bss();
int is_aligned_to(size_t value, size_t size);
size_t align_up(size_t value, size_t size);
size_t align_down(size_t value, size_t size);
void page_init();
void page_free(void *addr);
HeapHeader *page_alloc();
void *kmalloc(size_t size);
void kheap_split(HeapHeader *header, size_t size);
void kheap_insert_sorted(HeapHeader *new_block);
void kfree(void *ptr);
HeapHeader *kcoalesce(HeapHeader *header);
void test_memory_reuse();
void test_memory_integrity();
void test_memory_alignment();
void test_memory_stress();
void heap_stats();

#endif
