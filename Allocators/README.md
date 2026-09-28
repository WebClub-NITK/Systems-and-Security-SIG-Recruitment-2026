# Lock-Free Concurrency & Custom Allocators

**Tags:** `Lock-Free Concurrency` · `Memory Management` · `Systems Architecture` · `C++`

## Introduction

In latency-critical applications—such as userspace network bypass architectures, real-time telemetry systems, or high-throughput data pipelines—general-purpose dynamic memory allocators (`malloc`/`free`) and OS-level mutex locks can introduce unpredictable jitter, lock contention, and cache misses.

High-throughput systems rely on **pre-allocated contiguous memory arenas** and **lock-free ring buffers** with explicit atomic memory ordering to process millions of events per second with sub-microsecond latency.

This task requires building the foundational memory and concurrency primitives necessary for such high-performance engines from scratch, and then combining them into a unified message-passing pipeline.

---

## Problem Statements

### Phase 1: Contiguous Arena Allocator

Implement a custom **arena (bump) allocator** in C or C++ that bypasses standard heap allocations.

Requirements:

- Request a large chunk of virtual memory directly from the operating system using `mmap()` (or `VirtualAlloc` on Windows).

- Provide aligned allocation routines:

  ```cpp
  void* alloc(size_t size, size_t alignment);
  ```

- The allocator should have **zero internal fragmentation overhead**.

- Implement a batch deallocation mechanism that resets the allocator state by moving the bump pointer back to the start.

- The reset operation must run in **O(1)** time.

---

### Phase 2: Cache-Conscious SPSC Ring Buffer

Design and implement a bounded **Single-Producer Single-Consumer (SPSC) circular queue** designed to pass memory pointers.

Requirements:

- The implementation must be entirely **lock-free**.
- Do **not** use:
  - `std::mutex`
  - Semaphores
  - Condition variables

- Utilize `std::atomic` and explicit memory ordering, including:
  - `std::memory_order_acquire`
  - `std::memory_order_release`

- Structure the queue to prevent **false sharing**.
- Ensure the `head` and `tail` atomic pointers reside on distinct CPU cache lines (typically 64 bytes) using alignment directives such as:

  ```cpp
  alignas(64)
  ```

---

### Phase 3: Integrated Low-Latency Pipeline

Combine the arena allocator and lock-free queue to simulate a deterministic, high-throughput **network packet processor** or **data ingestion daemon**.

#### Producer Thread

The producer should:

1. Rapidly generate simulated incoming data streams, such as `PacketMetadata` or `SensorReading` structs.
2. Allocate memory for each struct **exclusively from the Phase 1 Arena Allocator**.
3. Avoid using `new` or `malloc`.
4. Populate each struct with mock payload data.
5. Push the resulting pointer onto the Phase 2 SPSC queue.

#### Consumer Thread

The consumer should:

1. Continuously poll the SPSC queue.
2. Pop pointers from the queue.
3. Read the event data.
4. Compute a running tally or checksum.
5. Process messages in batches.

#### Arena Recycling

Once a large batch of messages has been fully processed and the queue is empty:

- Safely trigger the arena's **O(1) reset function**.
- Recycle the memory for the next burst of traffic.

#### Benchmarking

Benchmark the end-to-end throughput of the integrated pipeline:

> **Messages processed per second**

Compare the results against a baseline pipeline using:

- Standard `malloc()` / `free()`
- A `std::mutex`-backed `std::queue`

The benchmark should clearly demonstrate the performance characteristics of both implementations.

---

## Bonus Tasks

### MPMC Extension

Extend the queue design to support a **Multi-Producer Multi-Consumer (MPMC)** architecture using atomic Compare-And-Swap operations such as:

```cpp
compare_exchange_weak()
```

### Huge Pages

Configure the arena allocator to back its memory pool with **OS Huge Pages**, such as:

- 2 MB pages
- 1 GB pages

on Linux.

The goal is to minimize **TLB (Translation Lookaside Buffer) misses**.

### Sanitizer Integration

Integrate **ThreadSanitizer (TSan)** into the build pipeline to detect and verify the absence of data races in the lock-free logic.

---

## Resources

- _C++ Concurrency in Action_ — Anthony Williams
- Herb Sutter — _Atomic Weapons: The C++ Memory Model and Hardware_
- **1024cores** — Dmitry Vyukov's articles on lock-free algorithms and SPSC queue design
- Linux `mmap()` and `madvise()` documentation

---

## Submission:

- Create a private GitHub repository with solutions for each of the parts. The repository must be private until asked to be made public.
- Add mentors as collaborators.
- Include a README file explaining the implementation process, supported by screenshots and shell logs wherever necessary. Document failures(if any) and the resolution steps.
- Include a `Makefile` or `CMakeLists.txt` so that the benchmarks and tests can be compiled easily.

### Mentors:

1. Udbhav Naveen Kumar (Ph. No.: +91 9886144805, GitHub ID: Udbhav2006)
2. Abhyuday Hegde (Ph. No.: +91 8073294607, Github ID: akh7177)
3. Shanjiv A (Ph. No,: +91 9731362003, Github ID: shanjiv177)