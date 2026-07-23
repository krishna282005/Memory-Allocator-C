#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "heap.h"

#define BENCHMARK_RUNS 100
#define NUM_ALLOCS 10000
#define BLOCK_SIZE 32
#define MIN_SIZE 8
#define MAX_SIZE 512

void benchmark_fixed_size(void)
{
    void *ptrs[NUM_ALLOCS];

    printf("\n============================================\n");
    printf("Sequential Fixed Size Benchmark\n");
    printf("============================================\n");

    /*-------------------- kmalloc --------------------*/


    heap_init();

    clock_t start = clock();

    int successful = 0;

    for (int r = 0; r < BENCHMARK_RUNS; r++)
    {

        heap_init();

        successful = 0;

        for (int i = 0; i < NUM_ALLOCS; i++)
        {
            ptrs[i] = kmalloc(BLOCK_SIZE);

            if (ptrs[i] == NULL)
                break;

            successful++;
        }

        for (int i = 0; i < successful; i++)
        {
            kfree(ptrs[i]);
        }
    }

    clock_t end = clock();

    double kmalloc_total_time =
        ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

    printf("\nkmalloc()\n");
    printf("----------------------------\n");
    printf("Successful Allocations : %d\n", successful);
    printf("Total Time             : %.3f ms\n", kmalloc_total_time);
    printf("Average Time/Run       : %.6f ms\n",
        kmalloc_total_time / BENCHMARK_RUNS);


    /*-------------------- malloc --------------------*/

    start = clock();

    for (int r = 0; r < BENCHMARK_RUNS; r++)
    {
        for (int i = 0; i < NUM_ALLOCS; i++)
        {
            ptrs[i] = malloc(BLOCK_SIZE);
        }

        for (int i = 0; i < NUM_ALLOCS; i++)
        {
            free(ptrs[i]);
        }
    }

    end = clock();

    double malloc_total_time =
    ((double)(end-start)*1000.0)/CLOCKS_PER_SEC;

    printf("\nmalloc()\n");
    printf("----------------------------\n");
    printf("Successful Allocations : %d\n", NUM_ALLOCS);
    printf("Total Time             : %.3f ms\n", malloc_total_time);
    printf("Average Time/Run       : %.6f ms\n",
        malloc_total_time/BENCHMARK_RUNS);

}

void benchmark_random_workload(void)
{
    void *ptrs[NUM_ALLOCS];
    int sizes[NUM_ALLOCS];
    int order[NUM_ALLOCS];

    srand(42);

    /* Generate random sizes */
    for (int i = 0; i < NUM_ALLOCS; i++)
        sizes[i] = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);

    printf("\n============================================\n");
    printf("Random Allocation + Random Free Benchmark\n");
    printf("============================================\n");

    /******************** kmalloc ********************/

    heap_init();

    int successful = 0;

    clock_t total_start = clock();

    /* Allocation */

    clock_t alloc_start = clock();

    for (int i = 0; i < NUM_ALLOCS; i++)
    {
        ptrs[i] = kmalloc(sizes[i]);

        if (ptrs[i] == NULL)
            break;

        order[successful] = successful;
        successful++;
    }

    clock_t alloc_end = clock();

    /* Shuffle free order */

    for (int i = successful - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    /* Free */

    clock_t free_start = clock();

    for (int i = 0; i < successful; i++)
        kfree(ptrs[order[i]]);

    clock_t free_end = clock();

    clock_t total_end = clock();

    printf("\nkmalloc()/kfree()\n");
    printf("---------------------------------\n");
    printf("Successful Allocations : %d\n", successful);
    printf("Allocation Time        : %.3f ms\n",
           ((double)(alloc_end - alloc_start) * 1000.0) / CLOCKS_PER_SEC);
    printf("Free Time              : %.3f ms\n",
           ((double)(free_end - free_start) * 1000.0) / CLOCKS_PER_SEC);
    printf("Total Time             : %.3f ms\n",
           ((double)(total_end - total_start) * 1000.0) / CLOCKS_PER_SEC);



    /******************** malloc ********************/

    int malloc_successful = successful;

    clock_t malloc_total_start = clock();

    alloc_start = clock();

    for (int i = 0; i < malloc_successful; i++)
        ptrs[i] = malloc(sizes[i]);

    alloc_end = clock();

    /* Same free order */

    free_start = clock();

    for (int i = 0; i < malloc_successful; i++)
        free(ptrs[order[i]]);

    free_end = clock();

    clock_t malloc_total_end = clock();

    printf("\nmalloc()/free()\n");
    printf("---------------------------------\n");
    printf("Successful Allocations : %d\n", malloc_successful);
    printf("Allocation Time        : %.3f ms\n",
           ((double)(alloc_end - alloc_start) * 1000.0) / CLOCKS_PER_SEC);
    printf("Free Time              : %.3f ms\n",
           ((double)(free_end - free_start) * 1000.0) / CLOCKS_PER_SEC);
    printf("Total Time             : %.3f ms\n",
           ((double)(malloc_total_end - malloc_total_start) * 1000.0) / CLOCKS_PER_SEC);
}


void benchmark_random_free(void)
{
    void *ptrs[NUM_ALLOCS];
    int sizes[NUM_ALLOCS];
    int order[NUM_ALLOCS];

    srand(42);

    /* Generate random sizes */
    for (int i = 0; i < NUM_ALLOCS; i++)
    {
        sizes[i] = MIN_SIZE + rand() % (MAX_SIZE - MIN_SIZE + 1);
    }

    printf("\n============================================\n");
    printf("Random Free Pattern Benchmark\n");
    printf("============================================\n");

    /**************** kmalloc ****************/

    heap_init();

    int successful = 0;

    for (int i = 0; i < NUM_ALLOCS; i++)
    {
        ptrs[i] = kmalloc(sizes[i]);

        if (ptrs[i] == NULL)
            break;

        order[successful] = successful;
        successful++;
    }

    /* Shuffle only allocated blocks */
    for (int i = successful - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    clock_t start = clock();

    for (int i = 0; i < successful; i++)
    {
        kfree(ptrs[order[i]]);
    }

    clock_t end = clock();

    double kfree_time =
        ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

    printf("\nkmalloc()/kfree()\n");
    printf("----------------------------\n");
    printf("Successful Allocations : %d\n", successful);
    printf("Random Free Time       : %.3f ms\n", kfree_time);
    int klocksucc = successful;

    /**************** malloc ****************/

    successful = 0;

    for (int i = 0; i < klocksucc; i++)
    {
        ptrs[i] = malloc(sizes[i]);

        if (ptrs[i] == NULL)
            break;

        order[successful] = successful;
        successful++;
    }

    /* Shuffle again */
    for (int i = successful - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int temp = order[i];
        order[i] = order[j];
        order[j] = temp;
    }

    start = clock();

    for (int i = 0; i < successful; i++)
    {
        free(ptrs[order[i]]);
    }

    end = clock();

    double free_time =
        ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

    printf("\nmalloc()/free()\n");
    printf("----------------------------\n");
    printf("Successful Allocations : %d\n", successful);
    printf("Random Free Time       : %.3f ms\n", free_time);
}