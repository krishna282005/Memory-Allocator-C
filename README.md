# Custom Heap Allocator & Benchmarking Framework in C

A custom dynamic memory allocator implemented from scratch in C to understand how `malloc()` and `free()` work internally. The project includes a complete benchmarking and analysis framework for evaluating allocator behavior under different memory workloads.

---

## Features

### Memory Allocator

- Custom `kmalloc()` and `kfree()` implementation
- First-Fit allocation strategy
- 8-byte memory alignment
- Block splitting
- Adjacent block coalescing
- Fixed-size heap management

### Debugging & Visualization

- Physical heap visualization
- Free list visualization
- Heap state inspection
- Allocation failure reporting

### Performance Analysis

- External fragmentation analysis
- Internal fragmentation analysis
- Peak heap utilization tracking
- Header overhead calculation
- Allocation failure statistics
- Largest free block tracking
- Total free memory tracking
- Fragmentation data export to CSV

### Benchmarking

- Fixed-size allocation workload
- Random allocation workload
- Growing/Shrinking allocation workload
- Stress-to-failure workload
- Latency benchmarking
- Workload performance comparison

---

## Project Structure

```text
Memory_Allocator_Project/
│
├── heap.c              # Core allocator implementation
├── heap.h
│
├── workload.c          # Workload generator
├── workload.h
│
├── benchmark.c         # Performance benchmarking
├── benchmark.h
│
├── metrics.c           # Fragmentation & utilization metrics
├── debug.c             # Heap visualization and debugging
│
├── main.c              # Driver program
│
├── README.md
└── .gitignore
```

---

## Workloads

The allocator is evaluated using multiple workload patterns that simulate different allocation behaviors.

| Workload | Description |
|----------|-------------|
| Fixed Size Churn | Repeated allocation and deallocation of equal-sized blocks |
| Random Churn | Random-sized allocations and frees |
| Growing & Shrinking | Heap gradually fills before being released |
| Stress to Failure | Allocates until heap exhaustion to evaluate failure behavior |

---

## Metrics Collected

For every workload, the framework records:

- Allocation attempts
- Successful allocations
- Allocation failures
- Peak heap utilization
- Internal fragmentation
- External fragmentation index
- Largest free block
- Total free memory
- Header overhead
- Latency benchmark
- Fragmentation history (CSV)

---

## Build

```bash
gcc -O2 main.c heap.c debug.c metrics.c workload.c benchmark.c -o allocator.exe
```

---

## Run

```bash
./allocator
```

---

## Sample Output

```text
========== Running workload: random_churn ==========

Peak Utilization      : 43.6%
Fragmentation Index   : 0.312
Largest Free Block    : 1200 bytes
Allocation Failures   : None

========== Latency Benchmark ==========
Average Time per Operation : 0.0086 us
```

---

## Learning Objectives

This project was built to gain a deeper understanding of:

- Dynamic memory allocation
- Heap management
- Pointer arithmetic
- Memory alignment
- Free-list management
- Block splitting and coalescing
- Heap fragmentation
- Memory allocator benchmarking
- Performance measurement
- Modular C project architecture

---

## Future Improvements

- Best-Fit allocation strategy
- Worst-Fit allocation strategy
- Next-Fit allocation strategy
- FreeRTOS `heap_4` integration
- Comparative allocator benchmarking
- Heap fragmentation visualization
- Performance graphs
- Allocation heat maps