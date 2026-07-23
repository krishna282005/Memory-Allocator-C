#include <stdio.h>
#include <stdint.h>
#include "heap.h"

static uint8_t fakeheap[1024];
static uint8_t minimum_space = 16;
BlockHeader *free_list_head = NULL;

///////////////////// Initialization ////////////////////





void heap_init()
{
    free_list_head = (BlockHeader*)fakeheap;
    
    free_list_head->size = sizeof(fakeheap) - sizeof(BlockHeader);
    free_list_head->is_free = 1;

    BlockHeader **next_address = (BlockHeader**)(free_list_head + 1);
    *next_address = NULL;
}


void* kmalloc(int sz)
{   
    BlockHeader *prev = NULL;
    BlockHeader *current = free_list_head;
    BlockHeader *leftover_address = NULL;

    int ac_size =  sz + ((sz%8 == 0)?(0):(8 - sz%8));
   

    while(current != NULL)
    {
        if (current->size >= ac_size && current->is_free == 1)
        {
            if ((current->size - ac_size)>minimum_space)
            {
                leftover_address = (BlockHeader *)((uint8_t *)(current + 1) + ac_size);
                leftover_address->size = current->size - ac_size - (int)sizeof(BlockHeader);
                leftover_address->is_free = 1;
                *(BlockHeader **)(leftover_address + 1) = *(BlockHeader **)(current + 1);
                current->size = ac_size;
            }
            break;
        }
        else
        {
            prev=current;
            current = *(BlockHeader **)(current + 1);
        }
    }

    if (current==NULL)
    {
        return NULL;
    }
    else
    {
        current->is_free=0;
        BlockHeader *current_next = (leftover_address != NULL)?(leftover_address):(*(BlockHeader **)(current + 1));
        if (prev!=NULL)
        {
            *(BlockHeader **)(prev + 1) = current_next;
        }
        else
        {
            free_list_head = current_next;
        }
    }
    return (void *)(current +1);

}


void kfree(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    BlockHeader *present = (BlockHeader *)ptr;
    BlockHeader *present_header = (BlockHeader *)(present - 1);
    BlockHeader *prev = NULL;
    BlockHeader *current = (BlockHeader *)free_list_head;
    while (current != NULL)
    {
        if (present_header < current)
        {
            break;
        }
        prev=current;
        current = *(BlockHeader **)(current + 1);
    }

    present_header->is_free=1;
    *(BlockHeader **)(present_header + 1) = current;

    if (prev != NULL)
    {
        *(BlockHeader **)(prev + 1) = present_header;
        uint8_t right = (((BlockHeader *)((uint8_t *)(present_header + 1) + present_header->size)) == current) ? 1 : 0 ;
        uint8_t left  = (((BlockHeader *)((uint8_t *)(prev + 1) + prev->size)) == present_header) ? 1 : 0 ;
        uint8_t tot = right + left ;
        switch(tot)
        {
            case 0:
                break;
            case 1:
                if (right)
                {
                    present_header->size = present_header->size + sizeof(BlockHeader) + current->size;
                    *(BlockHeader **)(present_header +1) = *(BlockHeader **)(current +1);
                    break;
                }
                else
                {
                    prev->size = prev->size + present_header->size + sizeof(BlockHeader);
                    *(BlockHeader **)(prev + 1) = current;
                    break;
                }
            case 2:
                prev->size = prev->size + present_header->size + current->size + 2 * sizeof(BlockHeader);
                *(BlockHeader **)(prev + 1) = *(BlockHeader**)(current+1);
                break;
        }
    }
    else
    {
        free_list_head = present_header;
        uint8_t right = (((BlockHeader *)((uint8_t *)(present_header + 1) + present_header->size)) == current) ? 1 : 0 ;

        if (right)
        {
            present_header->size = present_header->size + sizeof(BlockHeader) + current->size;
            *(BlockHeader **)(present_header +1) = *(BlockHeader **)(current +1);
        }

    }

}


void physical_heap_dump()
{
    BlockHeader *current = (BlockHeader *)fakeheap;
    BlockHeader *heap_end = (BlockHeader *)(fakeheap + sizeof(fakeheap));
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
    BlockHeader *current = (BlockHeader *)fakeheap;
    BlockHeader *heap_end = (BlockHeader *)(fakeheap + sizeof(fakeheap));
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
