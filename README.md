# Custom Heap Allocator in C

A custom dynamic memory allocator implemented from scratch in C to understand how `malloc()` and `free()` work internally.

## Features

- Custom `kmalloc()` and `kfree()`
- First-Fit allocation strategy
- 8-byte memory alignment
- Block splitting
- Adjacent block coalescing
- Physical heap visualization
- Free list visualization
- Heap fragmentation analysis
- Fragmentation data export to CSV

## Project Structure

```
Memory_Allocator_Project/
├── heap.c
├── heap.h
├── main.c
├── README.md
└── .gitignore
```

## Build

```bash
gcc main.c heap.c -o allocator
```

## Run

```bash
./allocator
```

## Learning Objectives

This project was built to gain a deeper understanding of:

- Dynamic memory allocation
- Pointer arithmetic
- Memory alignment
- Free-list management
- Heap fragmentation
- Multi-file C projects