#ifndef KMEMORY_H
#define KMEMORY_H

void page_init();
void page_free(void *addr);
void *page_alloc();

#endif
