# Custom Memory Allocator (mymalloc)

## Introduction
This project implements a custom memory allocator in C, providing `mymalloc` and `myfree` functions that mimic the behavior of the standard `malloc` and `free`. The allocator manages a fixed-size heap and includes mechanisms for memory allocation, deallocation, coalescing free blocks, and leak detection. Two test programs, `memtest` and `memgrind`, are provided to validate the allocator's correctness and measure its performance under various scenarios.

## Features
- **Fixed-size Heap**: Manages a heap of 4096 bytes.
- **Custom Allocation Functions**: Implements `mymalloc(size_t size, char *file, int line)` and `myfree(void *ptr, char *file, int line)`.
- **Memory Alignment**: Ensures all allocations are aligned to 8-byte boundaries.
- **Metadata Management**: Uses metadata structures to track allocated and free memory blocks.
- **Coalescing Free Blocks**: Adjacent free blocks are merged to reduce fragmentation.
- **Leak Detection**: Reports memory leaks upon program termination using `atexit()`.

## Files Included
- `mymalloc.h`: Header file with function declarations and macro definitions.
- `mymalloc.c`: Implementation of the custom memory allocator.
- `memtest.c`: Test program with various test cases to validate correctness.
- `memgrind.c`: Performance testing program that measures execution time of different allocation patterns.
- `Makefile`: Build script for easy compilation.
- `README.md`: Documentation and usage instructions for the project.

## Compilation Instructions
To compile the programs, use the provided Makefile. Open a terminal in the project directory and run:

```bash
make all
```
This command compiles the following executables:
```bash
memtest
memgrind
```
To clean up object files and executables:

```bash
make clean
```
## Usage Instructions

### Running memtest
The memtest program runs a series of test cases to validate the correctness of the custom memory allocator.

```bash
./memtest
```
Expected Output: The program will print messages indicating the progress and results of each test case, along with any error messages from the allocator.

### Running memgrind
The memgrind program measures the performance of the allocator by running several test cases that simulate different memory allocation patterns.

```bash
./memgrind
```
Expected Output: The program will output the total and average execution time for each test case, helping you assess the allocator's efficiency.

## Test Programs

### memtest

#### Purpose
Validates the allocator's correctness by running a series of test cases covering:

- Basic malloc and free operations.
- Edge cases and error handling.
- Memory alignment and integrity.
- No overwrite and no overlap between memory blocks.
- Metadata integrity.
- Coalescing of free memory blocks.
- Memory leak detection.

#### Key Test Cases

- **Basic malloc and free Operations**
  - Allocates memory of various sizes.
  - Writes data to the allocated memory.
  - Frees the memory.
  - Verifies that data can be written and read without errors.

- **Edge Cases and Error Handling**
  - Calls malloc(0) and expects NULL or graceful handling.
  - Attempts to allocate more than available memory.
  - Frees a pointer not allocated by mymalloc.
  - Frees an address in the middle of an allocated block.
  - Attempts double free on the same pointer.

- **Memory Alignment and Integrity**
  - Allocates memory sizes not multiples of 8 bytes.
  - Verifies that returned pointers are aligned to 8-byte boundaries.
  - Checks that data in one block does not affect other blocks.

- **No Overwrite and No Overlap**
  - Allocates multiple blocks and fills them with different data.
  - Verifies that each block retains its data without interference.

- **Metadata Integrity**
  - Allocates the maximum possible memory without exceeding the heap size.
  - Ensures that the allocation succeeds and metadata remains intact.

- **Coalescing Free Blocks**
  - Allocates several blocks and frees them in specific orders.
  - Attempts to allocate a larger block that requires coalescing.
  - Verifies that allocation succeeds after coalescing.

- **Memory Leak Detection**
  - Allocates memory without freeing it intentionally.
  - Checks if the allocator reports the leaked memory upon program termination.

### memgrind

#### Purpose
Measures the performance of the allocator under different memory allocation patterns.

#### Test Cases

- **Test A: Repeated Allocation and Immediate Free**
  - Allocates and immediately frees 1 byte of memory, 120 times per run.
  - Repeats this sequence 50 times.
  - Records the total and average execution time.

- **Test B: Sequential Allocations Followed by Frees**
  - Allocates 1 byte 120 times, storing the pointers.
  - Frees all allocated memory after all allocations are complete.
  - Repeats this sequence 50 times.
  - Records the total and average execution time.

- **Test C: Randomized Allocation and Freeing**
  - Randomly chooses between allocating or freeing memory.
  - Ensures that the number of frees does not exceed allocations.
  - Continues until 120 allocations have been made.
  - Repeats this sequence 50 times.
  - Records the total and average execution time.

- **Test D: Memory Exhaustion Test**
  - Continuously allocates 1 byte of memory until malloc returns NULL.
  - Frees all allocated memory afterward.
  - Repeats this sequence 50 times.
  - Records the total and average execution time.

- **Test E: Variable Size Allocations and Reverse Freeing**
  - Allocates 120 blocks of random sizes between 1 and 64 bytes.
  - Stores the pointers.
  - Frees the blocks in reverse order.
  - Repeats this sequence 50 times.
  - Records the total and average execution time.

## Implementation Details

### Custom Memory Allocator (mymalloc)

#### Heap Management
Uses a static array to simulate the heap.

#### Metadata Structure
Each memory block is preceded by a metaNode struct containing:
- valid: Indicates if the block is allocated (1) or free (0).
- payload: Size of the allocated memory.

#### Allocation Logic
- Searches for a suitable free block.
- Splits the block if there is enough space for a new metaNode and minimum payload.
- Returns a pointer to the payload area.

#### Deallocation Logic
- Validates the pointer to ensure it was allocated by mymalloc.
- Marks the block as free.
- Coalesces adjacent free blocks.

#### Error Handling
- Invalid Requests: Prints error messages to stderr with file name and line number.
- Out-of-Bounds Access: Prevents accessing memory outside the managed heap.
- Double Free and Invalid Free: Detects and reports errors when freeing invalid pointers.

### Known Issues and Limitations
- Fixed Heap Size: The allocator uses a fixed heap size of 4096 bytes. Allocation requests exceeding this size will fail.
- Thread Safety: The allocator is not thread-safe. Concurrent access from multiple threads may lead to undefined behavior.
- Memory Alignment: Aligns memory to 8-byte boundaries, which may result in slightly more memory usage due to padding.
- Error Messages: Error messages are printed to stderr for debugging purposes.

## Development and Testing

- Debugging: Use printf statements or a debugger (like gdb) to trace through the code if issues arise.
- Memory Analysis: Tools like valgrind can help detect memory leaks or invalid memory accesses.
- Performance: Compare the allocator's performance with the system's malloc by compiling memgrind without the custom allocator.

## Makefile Targets
- make all: Compiles memtest and memgrind.
- make memtest: Compiles only the memtest program.
- make memgrind: Compiles only the memgrind program.
- make clean: Removes compiled object files and executables.
