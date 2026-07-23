#include <stdio.h>
#include <stdint.h>
#include "heap.h"

float frag_index[100];
int count = 0;

void fragment_analyzer();
void test_no_merge(void);
void test_left_merge(void);
void test_right_merge(void);
void test_both_merge(void);
void export_fragmentation_csv(void);


int main()
{
    heap_init();

    int *mem1 = (int*)kmalloc(100);
    int *mem2 = (int*)kmalloc(27);
    int *mem3 = (int*)kmalloc(522);
    kfree(mem2);

    free_heap_dump();
    physical_heap_dump();
    fragment_analyzer();
    export_fragmentation_csv();
    

    return 0;
}

void fragment_analyzer()
{
    BlockHeader *current = (BlockHeader *)free_list_head;
    uint32_t total_free_memory = 0;
    uint32_t max_payload = 0;
    float fi=0;
    while(current != NULL)
    {
        max_payload = (current->size > max_payload)?(current->size):(max_payload);
        total_free_memory = total_free_memory + current->size;
        current = *(BlockHeader **)(current + 1);
    }
    if (total_free_memory == 0)
    {
        printf("\n NO FREE SPACE \n");
    }
    else
    {
        if (count<100)
        {
            frag_index[count]= 1 - (float)(max_payload)/total_free_memory;
            fi=frag_index[count];
        }
        else
        {
            int i=0;
            while(i<99)
            {
                frag_index[i]=frag_index[i+1];
                i++;
            }
            frag_index[99]=1 - (float)(max_payload)/total_free_memory;
            fi=frag_index[99];
        }
    }
    printf("\n Total free memory = %u",total_free_memory);
    printf("\n Largest free block size = %u",max_payload);
    printf("\n Fragmentation index = %0.3f and no. of iteration = %u\n", fi,count);
    count=count+1;
}

void test_no_merge(void)
{
    printf("\n========== Case 1 : No Merge ==========\n");

    heap_init();

    int *mem1 = (int *)kmalloc(100);
    int *mem2 = (int *)kmalloc(27);
    int *mem3 = (int *)kmalloc(522);

    (void)mem2;
    (void)mem3;

    physical_heap_dump();
    free_heap_dump();

    kfree(mem1);

    physical_heap_dump();
    free_heap_dump();
}

void test_left_merge(void)
{
    printf("\n========== Case 2 : Left Merge ==========\n");

    heap_init();

    int *mem1 = (int *)kmalloc(100);
    int *mem2 = (int *)kmalloc(27);
    int *mem3 = (int *)kmalloc(522);

    (void)mem3;

    physical_heap_dump();
    free_heap_dump();

    kfree(mem1);
    kfree(mem2);

    physical_heap_dump();
    free_heap_dump();
}

void test_right_merge(void)
{
    printf("\n========== Case 3 : Right Merge ==========\n");

    heap_init();

    int *mem1 = (int *)kmalloc(100);
    int *mem2 = (int *)kmalloc(27);
    int *mem3 = (int *)kmalloc(522);

    (void)mem1;
    (void)mem2;

    physical_heap_dump();
    free_heap_dump();

    kfree(mem3);

    physical_heap_dump();
    free_heap_dump();
}

void test_both_merge(void)
{
    printf("\n========== Case 4 : Both Merge ==========\n");

    heap_init();

    int *mem1 = (int *)kmalloc(100);
    int *mem2 = (int *)kmalloc(27);
    int *mem3 = (int *)kmalloc(522);

    physical_heap_dump();
    free_heap_dump();

    kfree(mem1);
    kfree(mem3);
    kfree(mem2);

    physical_heap_dump();
    free_heap_dump();
}

void export_fragmentation_csv(void)
{
    FILE *fp = fopen("fragmentation.csv", "w");

    if (fp == NULL)
    {
        printf("Unable to create CSV file.\n");
        return;
    }

    fprintf(fp, "Iteration,Fragmentation_Index\n");

    for (int i = 0; i < count && i < 100; i++)
    {
        fprintf(fp, "%d,%f\n", i, frag_index[i]);
    }

    fclose(fp);

    printf("\nfragmentation.csv generated successfully.\n");
}
