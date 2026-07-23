#include <stdio.h>
#include <stdint.h>
#include "heap.h"

extern BlockHeader *free_list_head;


void physical_heap_dump()
{
    uint8_t *fakeheap = heap_start();
    size_t size_of_heap = heap_size();
    BlockHeader *current = (BlockHeader *)fakeheap;
    BlockHeader *heap_end = (BlockHeader *)(fakeheap + size_of_heap);
    int i=0;
    printf("\n---------------Heap Data---------------\n");

    while (current < heap_end)
    {
        printf("Block %d\n",i+1);
        printf("Header at              : %p\n",current);
        printf("Payload at             : %p\n",current + 1);
        printf("Size of payload        : %d\n",current->size);
        printf("Total Size of block    : %zu\n",(size_t)current->size + sizeof(BlockHeader));
        printf("Status of Block        : %s\n\n",(current->is_free)?"Free":"Allocated");
        current = (BlockHeader *)((uint8_t *)(current + 1) + current->size);
        i++;
    }
    printf("----------------------------------------\n");
}

void free_heap_dump()
{
    BlockHeader *current = free_list_head;
    int i=0;
    printf("\n---------------Free Heap Data---------------\n");

    while (current != NULL)
    {
        printf("Block %d\n",i+1);
        printf("Header at              : %p\n",current);
        printf("Payload at             : %p\n",current + 1);
        printf("Size of payload        : %d\n",current->size);
        printf("Total Size of block    : %zu\n",(size_t)current->size + sizeof(BlockHeader));
        printf("Status of Block        : %s\n\n",(current->is_free)?"Free":"Allocated");
        current = *(BlockHeader **)(current + 1);
        i++;
    }
    printf("----------------------------------------\n");
}


void check_alignment()
{
    uint8_t *fakeheap = heap_start();
    size_t size_of_heap = heap_size();
    BlockHeader *current = (BlockHeader *)fakeheap;
    BlockHeader *heap_end = (BlockHeader *)(fakeheap + size_of_heap);
    int block_num = 0;
    int misaligned_count = 0;

    printf("\n---------------Alignment Check---------------\n");

    while (current < heap_end)
    {
        uintptr_t addr = (uintptr_t)current;
        int is_aligned = (addr % 8 == 0);

        printf("Block %d | Header at %p | %s\n",
               block_num + 1, current, is_aligned ? "ALIGNED" : "MISALIGNED");

        if (!is_aligned)
            misaligned_count++;

        current = (BlockHeader *)((uint8_t *)(current + 1) + current->size);
        block_num++;
    }

    printf("----------------------------------------\n");
    if (misaligned_count == 0)
        printf("All %d blocks are 8-byte aligned.\n", block_num);
    else
        printf("%d out of %d blocks are MISALIGNED.\n", misaligned_count, block_num);
    printf("----------------------------------------\n");
}
