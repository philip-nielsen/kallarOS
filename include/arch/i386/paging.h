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

void paging_init();
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);
void unmap_page(uint32_t virt_addr);
void alloc_page(uint32_t virtual_addr, uint32_t flags);

#endif