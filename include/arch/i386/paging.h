#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 4096

typedef uint32_t page_table_entry_t;
typedef uint32_t page_directory_entry_t;

// Page table flags
#define PTE_PRESENT 0x001
#define PTE_RW 0x002
#define PTE_USER 0x004
#define PTE_ACCESSED 0x020
#define PTE_DIRTY 0x040
#define PTE_FRAME_MASK 0xFFFFF000

// Page directory flags
#define PDE_PRESENT 0x001
#define PDE_RW 0x002
#define PDE_USER 0x004
#define PDE_SIZE_4MB 0x080
#define PDE_TABLE_MASK 0xFFFFF000

/**
 * paging_init() - Sets the internal paging flag to enabled.
 */
void paging_init(void);

/**
 * map_page() - Maps a specific virtual address to a physical address.
 * @virt_addr: The virtual address to be mapped.
 * @phys_addr: The physical address to map to.
 * @flags: The PTE flags to set for this page (e.g., PTE_PRESENT | PTE_RW).
 */
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);

/**
 * unmap_page() - Unmaps a specific virtual address.
 * @virt_addr: The virtual address to unmap.
 *
 * Context: Panics if the virtual address is not present in the page directory.
 */
void unmap_page(uint32_t virt_addr);

/**
 * alloc_page() - Allocates and maps a new physical frame to a virtual address.
 * @virt_addr: The virtual address to be mapped.
 * @flags: The PTE flags to set for this page.
 *
 * Automatically fetches the next free physical frame from the PMM.
 * Context: Panics if out of physical memory or if the address is already
 * mapped.
 */
void alloc_page(uint32_t virt_addr, uint32_t flags);

#endif