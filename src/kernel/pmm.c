#include "pmm.h"
#include "multiboot.h"
#include "panic.h"
#include "../drivers/vga.h"

// 4KB blocks
#define PAGE_SIZE 4096

#define PAGE_ALIGN_DOWN(addr) ((addr) & ~0xFFF)
#define PAGE_ALIGN_UP(addr) (((addr) + 0xFFF) & ~0xFFF)

typedef struct {
    uint32_t* array;
    uint32_t max_blocks;
    uint32_t used_blocks;
    uint32_t last_allocated_bit;
} pmm_state_t;

static pmm_state_t pmm;

extern uint32_t kernel_start;
extern uint32_t kernel_end;

static inline void bitmap_set(uint32_t bit) {
    pmm.array[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(uint32_t bit) {
    pmm.array[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_check(uint32_t bit) {
    return pmm.array[bit / 32] & (1 << (bit % 32));
}

void pmm_mark_used(uint32_t physical_addr) {
    uint32_t frame_index = physical_addr / 4096;
    if (frame_index >= pmm.max_blocks) return;
    
    if (!bitmap_check(frame_index)) { 
        bitmap_set(frame_index);
        pmm.used_blocks++;
    }
}

void pmm_mark_free(uint32_t physical_addr) {
    uint32_t frame_index = physical_addr / 4096;
    if (frame_index >= pmm.max_blocks) return;
    
    if (bitmap_check(frame_index)) { 
        bitmap_clear(frame_index);
        pmm.used_blocks--;
    } else {
        kprintf("WARNING: Double free detected at physical address 0x%x\n", physical_addr);
        panic("Double free");
    }
}

void pmm_init(multiboot_info_t* mbd, uint32_t bitmap_addr) {
    uint32_t mem_size_bytes = (mbd->mem_upper + 1024) * 1024;
    pmm.max_blocks = mem_size_bytes / 4096;
    pmm.used_blocks = pmm.max_blocks;
    pmm.array = (uint32_t*) bitmap_addr;
    
    for (uint32_t i = 0; i < pmm.max_blocks; i++) {
        bitmap_set(i);
    }

    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) mbd->mmap_addr;

    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length) {

        if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t region_start = PAGE_ALIGN_UP(mmap->addr);
            uint32_t region_end = PAGE_ALIGN_DOWN(mmap->addr + mmap->len);

            for (uint32_t i = region_start; i <  region_end; i += 4096) {
                uint32_t frame_index = i / 4096;
                if (bitmap_check(frame_index)) { 
                    bitmap_clear(frame_index);
                    pmm.used_blocks--;
                };
            }

        }
        mmap = (multiboot_memory_map_t*) ( (uint32_t)mmap + mmap->size + sizeof(mmap->size) );
    }

    uint32_t start_addr = PAGE_ALIGN_DOWN((uint32_t)&kernel_start);  
    uint32_t end_addr = PAGE_ALIGN_UP((uint32_t)&kernel_end);

    // Protect kernel
    for (uint32_t i = start_addr; i < end_addr; i += 4096) {
        pmm_mark_used(i);
    }

    // Protect bitmap
    uint32_t bitmap_size_bytes = pmm.max_blocks / 8;
    uint32_t bitmap_end = bitmap_addr + bitmap_size_bytes;
    for (uint32_t i = bitmap_addr; i < bitmap_end; i += 4096) {
        pmm_mark_used(i);
    }
    
    //Set first 1MB as used
    for (uint32_t i = 0; i < 0x100000; i += 4096) {
        pmm_mark_used(i);
    }
}

uint32_t pmm_alloc_frame() {
    for (uint32_t i = pmm.last_allocated_bit; i < pmm.max_blocks; i++) {
        if (!bitmap_check(i)) {
            pmm_mark_used(i * 4096);
            pmm.last_allocated_bit = i;
            return (uint32_t) i * 4096;
        }
    }

    for (uint32_t i = 0; i < pmm.last_allocated_bit; i++) {
        if (!bitmap_check(i)) {
            pmm_mark_used(i * 4096);
            pmm.last_allocated_bit = i;
            return (uint32_t) i * 4096;
        }
    }

    return 0;
}

void pmm_free_frame(void *physical_addr) {
    pmm_mark_free((uint32_t) physical_addr);
}