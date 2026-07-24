#ifndef WORKLOAD_H
#define WORKLOAD_H

#include <stdint.h>

#define MAX_OPS 200

typedef enum { OP_ALLOC, OP_FREE } OpType;

typedef struct
{
    OpType type;
    int    size;       /* used when type == OP_ALLOC */
    int    alloc_ref;  /* used when type == OP_FREE: index into the ops list
                           of the ALLOC this FREE corresponds to */
} Operation;

typedef struct
{
    Operation ops[MAX_OPS];
    int       count;
} Workload;

/* Generators - all seeded/deterministic so the SAME workload can be
   replayed identically against any allocator */
Workload gen_fixed_size_churn(int num_cycles, int block_size);
Workload gen_random_churn(int num_allocs, int min_size, int max_size, unsigned int seed);
Workload gen_growing_shrinking(int num_allocs, int start_size, int step);

typedef struct
{
    int total_ops;
    int alloc_attempts;
    int alloc_successes;
    int first_failure_op;      /* -1 if no failure ever happened */
    int failed_alloc_size;     /* size that caused the first failure, -1 if none */
    uint32_t peak_bytes_allocated;   /* highest concurrent allocated payload bytes seen */
    float    peak_utilization_pct;   /* peak_bytes_allocated / FAKE_HEAP_SIZE * 100 */
    uint32_t total_internal_frag_bytes; /* sum of (aligned_size - requested_size) over all successful allocs */
    float    avg_internal_frag_bytes;   /* total_internal_frag_bytes / alloc_successes */
    int      final_block_count;         /* number of blocks (free+allocated) in physical heap at end of run */
    float    header_overhead_pct;       /* (final_block_count * sizeof(BlockHeader)) / FAKE_HEAP_SIZE * 100 */
} WorkloadStats;

/* Runner - executes a workload against whatever kmalloc/kfree are linked in,
   calling fragment_analyzer() after every single operation.
   Returns stats on failure point and peak utilization. */
WorkloadStats run_workload(Workload *w, const char *label);

typedef struct
{
    long   total_calls;       /* alloc_calls + free_calls, across all repetitions */
    long   alloc_calls;
    long   free_calls;
    double avg_us_per_call;   /* batch-timed: total elapsed / total_calls */
    double total_elapsed_us;  /* raw elapsed time for the whole benchmark */
} LatencyStats;

/* Runs the SAME workload 'repetitions' times back-to-back (heap_init() between
   each pass). Times the ENTIRE benchmark as one block (not per-call) and
   divides by total operation count - this sidesteps clock()'s coarse
   resolution on Windows (~1ms ticks), which makes per-call timing produce
   mostly-zero readings with occasional 1ms noise spikes. Trade-off: this
   gives one combined avg-per-operation number rather than separate
   alloc/free averages. On STM32/ESP32 later, per-call DWT cycle timing
   becomes practical again and the alloc/free split can be restored. */
LatencyStats benchmark_latency(Workload *w, const char *label, int repetitions);

#endif