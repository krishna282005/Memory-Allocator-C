#include <stdio.h>
#include <stdint.h>
#include "heap.h"


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
