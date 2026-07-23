#include <stdio.h>
#include <stdint.h>
#include "heap.h"
#include "benchmark.h"



int main()
{
    heap_init();
    benchmark_random_workload();

    // int *mem1 = (int*)kmalloc(100);
    // int *mem2 = (int*)kmalloc(27);
    // int *mem3 = (int*)kmalloc(522);
    // kfree(mem2);

    // free_heap_dump();
    // physical_heap_dump();
    // fragment_analyzer();
    // export_fragmentation_csv();
    
    
    

    return 0;
}