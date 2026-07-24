#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#define HEAP_SIZE (2048)

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
void fragment_analyzer();
void test_no_merge(void);
void test_left_merge(void);
void test_right_merge(void);
void test_both_merge(void);
void export_fragmentation_csv(const char *filename);
uint8_t *heap_start (void);
size_t heap_size(void);

#endif
