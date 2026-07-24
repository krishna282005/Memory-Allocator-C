#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "workload.h"
#include "heap.h"
#include "metrics.h"

/* ---------- Workload 1: Fixed-size churn ----------
   alloc, free, alloc, free ... same size every time.
   Expected result: near-zero fragmentation throughout. */
Workload gen_fixed_size_churn(int num_cycles, int block_size)
{
    Workload w;
    w.count = 0;

    for (int i = 0; i < num_cycles && w.count + 1 < MAX_OPS; i++)
    {
        int alloc_idx = w.count;
        w.ops[w.count].type = OP_ALLOC;
        w.ops[w.count].size = block_size;
        w.count++;

        w.ops[w.count].type = OP_FREE;
        w.ops[w.count].alloc_ref = alloc_idx;
        w.count++;
    }
    return w;
}

/* ---------- Workload 2: Random churn ----------
   Random sizes, allocated all at once, then freed in random order.
   Seeded so it's reproducible across allocator runs. */
Workload gen_random_churn(int num_allocs, int min_size, int max_size, unsigned int seed)
{
    Workload w;
    w.count = 0;
    srand(seed);

    int alloc_indices[MAX_OPS];
    int n = (num_allocs < MAX_OPS / 2) ? num_allocs : MAX_OPS / 2;

    for (int i = 0; i < n; i++)
    {
        w.ops[w.count].type = OP_ALLOC;
        w.ops[w.count].size = min_size + (rand() % (max_size - min_size + 1));
        alloc_indices[i] = w.count;
        w.count++;
    }

    /* Fisher-Yates shuffle of alloc_indices to get a random free order */
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int tmp = alloc_indices[i];
        alloc_indices[i] = alloc_indices[j];
        alloc_indices[j] = tmp;
    }

    for (int i = 0; i < n && w.count < MAX_OPS; i++)
    {
        w.ops[w.count].type = OP_FREE;
        w.ops[w.count].alloc_ref = alloc_indices[i];
        w.count++;
    }

    return w;
}

/* ---------- Workload 3: Growing sizes, then scrambled free ----------
   Allocates increasing sizes, then frees every-other block first
   (leaving gaps), then fills in the rest. Deliberately adversarial -
   this is the pattern that produced your rise-then-fall fragmentation curve. */
Workload gen_growing_shrinking(int num_allocs, int start_size, int step)
{
    Workload w;
    w.count = 0;

    int alloc_indices[MAX_OPS];
    int n = (num_allocs < MAX_OPS / 2) ? num_allocs : MAX_OPS / 2;

    for (int i = 0; i < n; i++)
    {
        w.ops[w.count].type = OP_ALLOC;
        w.ops[w.count].size = start_size + i * step;
        alloc_indices[i] = w.count;
        w.count++;
    }

    /* free every-other block first (odd indices), then the rest (even indices) */
    for (int i = 1; i < n && w.count < MAX_OPS; i += 2)
    {
        w.ops[w.count].type = OP_FREE;
        w.ops[w.count].alloc_ref = alloc_indices[i];
        w.count++;
    }
    for (int i = 0; i < n && w.count < MAX_OPS; i += 2)
    {
        w.ops[w.count].type = OP_FREE;
        w.ops[w.count].alloc_ref = alloc_indices[i];
        w.count++;
    }

    return w;
}

/* ---------- Runner ---------- */
WorkloadStats run_workload(Workload *w, const char *label)
{
    void *ptrs[MAX_OPS] = {0};
    int   alloc_size_of[MAX_OPS] = {0};  /* size that op i actually allocated, 0 if none/freed */

    WorkloadStats stats;
    stats.total_ops = w->count;
    stats.alloc_attempts = 0;
    stats.alloc_successes = 0;
    stats.first_failure_op = -1;
    stats.failed_alloc_size = -1;
    stats.peak_bytes_allocated = 0;
    stats.total_internal_frag_bytes = 0;

    uint32_t current_allocated = 0;

    printf("\n========== Running workload: %s (%d ops) ==========\n", label, w->count);

    reset_fragmentation_log();

    for (int i = 0; i < w->count; i++)
    {
        if (w->ops[i].type == OP_ALLOC)
        {
            stats.alloc_attempts++;
            ptrs[i] = kmalloc(w->ops[i].size);

            if (ptrs[i] == NULL)
            {
                printf("Op %d: ALLOC %d bytes -> FAILED (out of memory)\n", i, w->ops[i].size);
                if (stats.first_failure_op == -1)
                {
                    stats.first_failure_op = i;
                    stats.failed_alloc_size = w->ops[i].size;
                }
            }
            else
            {
                stats.alloc_successes++;
                alloc_size_of[i] = w->ops[i].size;
                current_allocated += w->ops[i].size;
                if (current_allocated > stats.peak_bytes_allocated)
                {
                    stats.peak_bytes_allocated = current_allocated;
                }

                /* Internal fragmentation: wasted bytes from 8-byte rounding.
                   Must match the exact rounding formula used inside kmalloc(). */
                int sz = w->ops[i].size;
                int ac_size = sz + ((sz % 8 == 0) ? 0 : (8 - sz % 8));
                stats.total_internal_frag_bytes += (uint32_t)(ac_size - sz);
            }
        }
        else /* OP_FREE */
        {
            int ref = w->ops[i].alloc_ref;
            if (ptrs[ref] != NULL)
            {
                kfree(ptrs[ref]);
                current_allocated -= alloc_size_of[ref];
                ptrs[ref] = NULL;
                alloc_size_of[ref] = 0;
            }
        }
        fragment_analyzer();
    }
    int FAKE_HEAP_SIZE = heap_size();
    uint8_t *fakeheap = heap_start();

    stats.peak_utilization_pct = 100.0f * (float)stats.peak_bytes_allocated / (float)FAKE_HEAP_SIZE;
    stats.avg_internal_frag_bytes = (stats.alloc_successes > 0)
        ? ((float)stats.total_internal_frag_bytes / stats.alloc_successes)
        : 0.0f;

    /* Header overhead: walk the physical heap (not just free list) to count
       EVERY block currently in existence, free or allocated - every block
       costs one header regardless of its state. */
    {
        BlockHeader *cur = (BlockHeader *)fakeheap;
        BlockHeader *heap_end = (BlockHeader *)(fakeheap + FAKE_HEAP_SIZE);
        int block_count = 0;
        while (cur < heap_end)
        {
            block_count++;
            cur = (BlockHeader *)((uint8_t *)(cur + 1) + cur->size);
        }
        stats.final_block_count = block_count;
        stats.header_overhead_pct = 100.0f * (float)(block_count * (int)sizeof(BlockHeader)) / (float)FAKE_HEAP_SIZE;
    }

    char filename[64];
    snprintf(filename, sizeof(filename), "%s.csv", label);
    export_fragmentation_csv(filename);

    printf("\n---- Workload '%s' summary ----\n", label);
    printf("Total ops           : %d\n", stats.total_ops);
    printf("Alloc attempts      : %d\n", stats.alloc_attempts);
    printf("Alloc successes     : %d\n", stats.alloc_successes);
    if (stats.first_failure_op == -1)
        printf("Allocation failures : none\n");
    else
        printf("First failure at op : %d (requested %d bytes)\n", stats.first_failure_op, stats.failed_alloc_size);
    printf("Peak bytes allocated: %u / %d\n", stats.peak_bytes_allocated, FAKE_HEAP_SIZE);
    printf("Peak utilization    : %.1f%%\n", stats.peak_utilization_pct);
    printf("Internal frag total : %u bytes (avg %.2f bytes/alloc, from 8-byte rounding)\n",
           stats.total_internal_frag_bytes, stats.avg_internal_frag_bytes);
    printf("Header overhead     : %d blocks x %zu bytes = %.1f%% of heap\n",
           stats.final_block_count, sizeof(BlockHeader), stats.header_overhead_pct);
    printf("========== Workload '%s' complete ==========\n", label);

    return stats;
}

/* ---------- Latency benchmark ----------
   Replays the SAME workload 'repetitions' times, timing each individual
   kmalloc/kfree call. clock() resolution on PC is coarse (often ~10-15ms
   ticks on Windows), so a single call usually reads as ~0us - that's a
   real limitation of PC timing, not a bug. Averaging over many calls
   (repetitions * ops_per_workload) smooths this out into a meaningful
   number. On STM32/ESP32 later, this same structure gets reused with the
   DWT cycle counter in place of clock() for exact per-call cycle counts. */
LatencyStats benchmark_latency(Workload *w, const char *label, int repetitions)
{
    void *ptrs[MAX_OPS] = {0};
    long alloc_calls = 0, free_calls = 0;

    printf("\n========== Latency benchmark: %s (%d reps x %d ops) ==========\n",
           label, repetitions, w->count);

    double total_elapsed_us = 0.0;

    for (int r = 0; r < repetitions; r++)
    {
        heap_init();  /* reset cost excluded from timing - not a kmalloc/kfree op */

        clock_t t_start = clock();

        for (int i = 0; i < w->count; i++)
        {
            if (w->ops[i].type == OP_ALLOC)
            {
                ptrs[i] = kmalloc(w->ops[i].size);
                alloc_calls++;
            }
            else /* OP_FREE */
            {
                int ref = w->ops[i].alloc_ref;
                if (ptrs[ref] != NULL)
                {
                    kfree(ptrs[ref]);
                    ptrs[ref] = NULL;
                    free_calls++;
                }
            }
        }

        clock_t t_end = clock();
        total_elapsed_us += ((double)(t_end - t_start) * 1000000.0) / CLOCKS_PER_SEC;
    }

    long   total_calls = alloc_calls + free_calls;

    LatencyStats stats;
    stats.total_calls      = total_calls;
    stats.alloc_calls      = alloc_calls;
    stats.free_calls       = free_calls;
    stats.total_elapsed_us = total_elapsed_us;
    stats.avg_us_per_call  = (total_calls > 0) ? (total_elapsed_us / total_calls) : 0.0;

    printf("---- Latency summary: %s ----\n", label);
    printf("Total operations timed : %ld  (%ld kmalloc + %ld kfree)\n",
           stats.total_calls, stats.alloc_calls, stats.free_calls);
    printf("Total elapsed time     : %.1f us  (%.3f ms)\n",
           stats.total_elapsed_us, stats.total_elapsed_us / 1000.0);
    printf("Avg time per operation : %.4f us\n", stats.avg_us_per_call);
    printf("NOTE: batch-timed (entire benchmark as one block) to avoid clock()'s\n");
    printf("      coarse per-call resolution on Windows. Reports one combined\n");
    printf("      avg-per-operation figure rather than separate alloc/free\n");
    printf("      averages; the split becomes practical again with the DWT\n");
    printf("      cycle counter once ported to STM32/ESP32.\n");
    printf("========== Latency benchmark '%s' complete ==========\n", label);

    return stats;
}