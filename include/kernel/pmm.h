#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <arch/i386/multiboot.h>

void pmm_init(multiboot_info_t* mbd, uint32_t bitmap_addr);
void pmm_mark_used(uint32_t physical_addr);
void pmm_mark_free(uint32_t physical_addr);
uint32_t pmm_alloc_frame(); // Returns the address of a free 4KB frame
void pmm_free_frame(void* physical_addr);

#endif