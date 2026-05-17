#ifndef KMALLOC_H
#define KMALLOC_H
#include <stddef.h>
#include <stdint.h>

/**
 * kmalloc() - Allocates the desired number of bytes on the kernel heap.
 * @bytes_to_allocate: The number of bytes to allocate.
 *
 * Performs a first-fit allocation. If no suitable free block is found,
 * and there is insufficient space left on the heap, new physical pages are
 * automatically mapped to expand the heap.
 * The returned memory block is guaranteed to be 16-byte aligned.
 *
 * Return: The virtual address to the beginning of the allocated block.
 * Returns 0 if bytes_to_allocate is 0.
 */
uint32_t kmalloc(size_t bytes_to_allocate);

/**
 * kfree() - Frees an allocated block on the kernel heap.
 * @address: The virtual address of the memory block to be freed.
 *
 * Marks the data block as free and automatically merges it with adjacent
 * free blocks to reduce heap fragmentation before adding it back to the free
 * list.
 *
 * Context: Causes a kernel panic if trying to free memory that is already
 * free (use-after-free/double-free prevention).
 */
void kfree(uint32_t address);

/**
 * kmalloc_init() - Initializes the kernel heap manager.
 *
 * Sets the initial heap watermark to the correct position and aligns it
 * properly. The heap is initialized to start safely 4 MB after the kernel end
 * address.
 */
void kmalloc_init(void);

#endif