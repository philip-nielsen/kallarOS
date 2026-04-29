#include <arch/i386/paging.h>
#include <kernel/pmm.h>
#include <kernel/panic.h>
#include <libc/string.h>
#include <stdint.h>

#define RECURSIVE_DIR_PTR  ((page_directory_entry_t*) 0xFFFFF000)
#define RECURSIVE_TABLE_PTR(dir_index) ((page_table_entry_t*) (0xFFC00000 + ((dir_index) * 0x1000)))

static page_directory_entry_t* kernel_directory = 0;
static int paging_enabled = 0;

void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    uint32_t dir_index = virt_addr >> 22;
    uint32_t tab_index = (virt_addr >> 12) & 0x3FF;

    if (paging_enabled) {
        if (!(RECURSIVE_DIR_PTR[dir_index] & PDE_PRESENT)) {
            uint32_t new_frame = pmm_alloc_frame();
            uint32_t dir_flags = PDE_PRESENT | PDE_RW;

            if (flags & PTE_USER) {
                dir_flags |= PDE_USER;
            }
            
            RECURSIVE_DIR_PTR[dir_index] = new_frame | dir_flags;

            __asm__ volatile("invlpg (%0)":: "r"(RECURSIVE_TABLE_PTR(dir_index)) : "memory");

            memset((void*)RECURSIVE_TABLE_PTR(dir_index), 0, 4096);
        }
        
    RECURSIVE_TABLE_PTR(dir_index)[tab_index] = phys_addr | flags;

    } else {
        if (!(kernel_directory[dir_index] & PDE_PRESENT)) {
            uint32_t new_frame = pmm_alloc_frame();
            uint32_t dir_flags = PDE_PRESENT | PDE_RW;

            memset((void*)new_frame, 0, 4096);

            if (flags & PTE_USER) {
                dir_flags |= PDE_USER;
            }
            kernel_directory[dir_index] = new_frame | dir_flags;
        }
        page_table_entry_t* table = (page_table_entry_t*) (kernel_directory[dir_index] & PDE_TABLE_MASK);

        table[tab_index] = phys_addr | flags;   
    }
}

void alloc_page(uint32_t virt_addr, uint32_t flags) {
    if (paging_enabled) {
        uint32_t dir_index = virt_addr >> 22;
        uint32_t tab_index = (virt_addr >> 12) & 0x3FF;

        if ((RECURSIVE_DIR_PTR[dir_index] & PDE_PRESENT) && 
            (RECURSIVE_TABLE_PTR(dir_index)[tab_index] & PTE_PRESENT)) {
            panic("Virtual address already mapped!\n");
        }

        uint32_t address = pmm_alloc_frame();
        if (address == 0) {
            panic("Out of RAM\n");
        }
        map_page(virt_addr, address, flags);
    } else {
        panic("Paging not enabled!\n");
    }
}

void free_page(uint32_t virt_addr) {
    if (paging_enabled) {
        uint32_t dir_index = virt_addr >> 22;
        uint32_t tab_index = (virt_addr >> 12) & 0x3FF;

        if (!(RECURSIVE_DIR_PTR[dir_index] & PDE_PRESENT)) {
            panic("Page Directory Entry not present!\n");
        }

        if (!(RECURSIVE_TABLE_PTR(dir_index)[tab_index] & PTE_PRESENT)) {
            panic("Page Table Entry not present!\n");
        }

        uint32_t frame_phys = RECURSIVE_TABLE_PTR(dir_index)[tab_index] & PTE_FRAME_MASK;
        pmm_free_frame((void*) frame_phys);
        unmap_page(virt_addr);
    } else {
        panic("Paging not enabled!\n");
    }
}

void unmap_page(uint32_t virt_addr) {
    if (paging_enabled) {
        uint32_t dir_index = virt_addr >> 22;
        uint32_t tab_index = (virt_addr >> 12) & 0x3FF;

        if (!(RECURSIVE_DIR_PTR[dir_index] & PDE_PRESENT)) {
            return; // Not present
        }

        if (RECURSIVE_TABLE_PTR(dir_index)[tab_index] & PTE_PRESENT) {

            RECURSIVE_TABLE_PTR(dir_index)[tab_index] = 0x0;

            __asm__ volatile("invlpg (%0)":: "r"(virt_addr) : "memory");
        }
    } else {
        panic("Paging not enabled!\n");
    }
    
}


void paging_init() {
    extern uint32_t kernel_end;
    kernel_directory = (page_directory_entry_t*) pmm_alloc_frame();
    memset(kernel_directory, 0, 4096); 
    for(uint32_t i = 0; i < (uint32_t)&kernel_end + 0x40000; i += 0x1000) {
        map_page(i, i, PTE_PRESENT | PTE_RW);
    }

    kernel_directory[1023] = (uint32_t)kernel_directory | PDE_PRESENT | PDE_RW;

    // Enabling paging
    // Load the physical address of the directory into CR3
    __asm__ volatile("mov %0, %%cr3":: "r"(kernel_directory));

    // Read CR0, flip the 31st bit (0x80000000), and write it back
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0":: "r"(cr0));
    paging_enabled = 1;
}