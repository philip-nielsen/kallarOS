#ifndef PMM_H
#define PMM_H

#include <arch/i386/multiboot.h>
#include <stdint.h>

/**
 * pmm_init() - Initializes the Physical Memory Manager (PMM).
 * @mbd: Pointer to the multiboot information structure provided by GRUB.
 * @bitmap_addr: The physical address where the PMM bitmap will be stored.
 *
 * This initializes the bitmap structure. It employs a whitelist approach:
 * it initially marks all memory as used for safety, and then selectively
 * frees only the regions explicitly guaranteed to be available by the
 * multiboot memory map.
 *
 * It explicitly protects the kernel code, the bitmap itself, and the first
 * 1MB of memory (BIOS/VGA) from being allocated.
 */
void pmm_init(multiboot_info_t *mbd, uint32_t bitmap_addr);

/**
 * pmm_mark_used() - Flags a specific physical frame as in-use.
 * @physical_addr: The physical address falling within the target 4KB frame.
 */
void pmm_mark_used(uint32_t physical_addr);

/**
 * pmm_alloc_frame() - Allocates the next available 4KB frame of physical
 * memory.
 * Scans the bitmap to find the first available free frame, marks it as used,
 * and returns its address. Searches in 32-bit chunks and uses GCC's
 * __builtin_ctz to instantly find the first free bit within the chunk.
 *
 * Return: The physical address of the allocated 4KB frame, or 0 if out of
 * memory.
 */
uint32_t pmm_alloc_frame(void);

/**
 * pmm_free_frame() - Releases a previously allocated 4KB physical frame.
 * @physical_addr: The 32-bit physical address of the frame to free.
 *
 * Context: Causes a kernel panic if trying to free a frame that is
 * already marked as free (double-free protection).
 */
void pmm_free_frame(uint32_t physical_addr);

#endif