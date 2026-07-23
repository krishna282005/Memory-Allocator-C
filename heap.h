#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

typedef struct
{
    uint32_t size;
    uint8_t is_free;
}BlockHeader;

extern BlockHeader *free_list_head;

void heap_init();
void* kmalloc(int sz);
void kfree(void *ptr);
void physical_heap_dump();
void free_heap_dump();
uint8_t *heap_start (void);
size_t heap_size(void);

#endif
