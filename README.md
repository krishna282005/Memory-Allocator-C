# Custom Heap Allocator in C

A custom dynamic memory allocator implemented from scratch in C to understand the internal working of dynamic memory management. The project implements a First-Fit allocator with block splitting, coalescing, alignment, heap inspection utilities, and a benchmarking framework for evaluating allocator behavior under different memory allocation workloads.

---

## Overview

```
                   +----------------------+
                   |     User Program     |
                   +----------+-----------+
                              |
                     kmalloc() / kfree()
                              |
                              v
                  +-------------------------+
                  |   Custom Heap Manager   |
                  +-------------------------+
                  |  First-Fit Allocation   |
                  |  Block Splitting        |
                  |  Block Coalescing       |
                  |  Free List Management   |
                  |  8-byte Alignment       |
                  +------------+------------+
                               |
                               v
                      Fixed Size Heap Buffer
```

---

# Features

## Memory Allocator

- Custom `kmalloc()` implementation
- Custom `kfree()` implementation
- First-Fit allocation strategy
- Fixed-size heap management
- 8-byte memory alignment
- Block splitting
- Adjacent free block coalescing
- Free-list based allocator

---

## Heap Analysis

- Physical heap visualization
- Free-list visualization
- Total free memory calculation
- Largest free block calculation
- External fragmentation analysis
- Internal fragmentation analysis
- Heap utilization statistics
- Header overhead analysis

---

## Benchmarking

The project includes a benchmarking module to evaluate allocator behavior under multiple workload patterns.

| Workload | Purpose |
|----------|----------|
| Fixed Size Churn | Repeated allocation/free of equal-sized blocks |
| Random Churn | Random-sized allocations and deallocations |
| Growing & Shrinking | Progressive heap fill followed by release |
| Stress to Failure | Allocation until heap exhaustion |

Each workload records:

- Allocation attempts
- Successful allocations
- Allocation failures
- Peak heap utilization
- Fragmentation index
- Internal fragmentation
- Header overhead
- Largest free block
- Total free memory
- Latency measurements
- CSV export for fragmentation history

---

# Allocator Design

```
               Heap Layout

+-------------------------------------------------------------+
| Header | Payload | Header | Payload | Header | Payload | ... |
+-------------------------------------------------------------+

Header
------
Size
Free/Allocated Flag
--> Next header pointer stored in payload (temp)
```

---

# Allocation Flow

```
kmalloc(size)
      |
      v
Search Free List (First-Fit)
      |
      +-------------------------+
      |                         |
 Found Suitable Block?        No
      |                         |
     Yes                        |
      |                         |
Split Block (if required)   Allocation Failed
      |
Mark Allocated
      |
Return Payload Pointer
```

---

# Free Operation

```
kfree(ptr)
     |
     v
Mark Block Free
     |
Insert Into Free List
     |
Merge Adjacent Blocks
     |
Update Heap Statistics
```

---

# Project Structure

```
Memory_Allocator_Project/

├── heap.c
├── heap.h
│
├── workload.c
├── workload.h
│
├── benchmark.c
├── benchmark.h
│
├── metrics.c
├── debug.c
│
├── main.c
│
├── README.md
└── .gitignore
```

---

# Build

```bash
gcc -O2 main.c heap.c debug.c metrics.c workload.c benchmark.c -o allocator.exe
```

---

# Run

```bash
./allocator
```

---

# Example Output

```
========== Running workload: random_churn ==========

Peak Heap Utilization : 43.6%
Largest Free Block    : 1200 bytes
Fragmentation Index   : 0.312
Allocation Failures   : None

Latency Benchmark

Average Time per Operation : 0.0086 us

Fragmentation history exported to:

fragmentation.csv
```

---

# Learning Outcomes

This project was built to gain practical understanding of:

- Dynamic memory allocation
- Heap organization
- Pointer arithmetic
- Memory alignment
- Free-list management
- Block splitting
- Block coalescing
- Heap fragmentation
- Performance benchmarking
- Modular software design in C

---

# Current Implementation

| Component | Status |
|-----------|--------|
| First-Fit Allocator | Complete |
| Heap Initialization | Complete |
| Block Splitting | Complete |
| Block Coalescing | Complete |
| Heap Visualization | Complete |
| Free List Visualization | Complete |
| Fragmentation Metrics | Complete |
| Benchmark Workloads | Complete |
| Latency Benchmarking | Complete |
| CSV Export | Complete |

---

# Future Work

The current implementation focuses on a First-Fit allocator. Possible future extensions include:

- Best-Fit allocation
- Worst-Fit allocation
- Next-Fit allocation
- FreeRTOS `heap_4` integration
- Comparative allocator evaluation
- Heap fragmentation visualization
- Benchmark result plotting
- Allocation heat-map generation

---

# License

This project is intended for educational purposes to explore the implementation and evaluation of custom memory allocators in C.