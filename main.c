#include <stdio.h>
#include <stdint.h>
#include "heap.h"
#include "workload.h"



int main()
{
    heap_init();
    Workload w1 = gen_fixed_size_churn(10, 40);        // 10 alloc/free cycles, 40 bytes each
    run_workload(&w1, "fixed_size_churn");

    heap_init();
    Workload w2 = gen_random_churn(15, 20, 100, 42);   // 15 allocs, size 20-100, seed 42
    run_workload(&w2, "random_churn");

    heap_init();
    Workload w3 = gen_growing_shrinking(15, 20, 15);   // 15 allocs, starting at 20, +15 each time
    run_workload(&w3, "growing_shrinking");

    heap_init();
    Workload w4 = gen_random_churn(40, 80, 200, 7);
    run_workload(&w4, "stress_to_failure");

    benchmark_latency(&w1, "fixed_size_churn_latency", 500000);
    benchmark_latency(&w2, "random_churn_latency", 500000);
    benchmark_latency(&w3, "growing_shrinking_latency", 500000);

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