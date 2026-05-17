#include <kernel/panic.h>
#include <kernel/pmm.h>
#include <libc/stdio.h>
#include <libc/string.h>

// 4KB blocks
#define PAGE_SIZE 4096

#define PAGE_ALIGN_DOWN(addr) ((addr) & ~0xFFF)
#define PAGE_ALIGN_UP(addr) (((addr) + 0xFFF) & ~0xFFF)

typedef struct {
    uint32_t *array;
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
    if (frame_index >= pmm.max_blocks)
        return;

    if (!bitmap_check(frame_index)) {
        bitmap_set(frame_index);
        pmm.used_blocks++;
    }
}

void pmm_free_frame(uint32_t physical_addr) {
    uint32_t frame_index = physical_addr / 4096;
    if (frame_index >= pmm.max_blocks)
        return;

    if (bitmap_check(frame_index)) {
        bitmap_clear(frame_index);
        pmm.used_blocks--;
    } else {
        printf("WARNING: Double free detected at physical address 0x%x\n",
               physical_addr);
        panic("Double free");
    }
}

static void pmm_mark_region_used(uint32_t start_addr, uint32_t end_addr) {
    uint32_t start_frame = PAGE_ALIGN_DOWN(start_addr) / PAGE_SIZE;
    uint32_t end_frame = PAGE_ALIGN_UP(end_addr) / PAGE_SIZE;

    for (uint32_t i = start_frame; i < end_frame; i++) {
        pmm_mark_used(i * PAGE_SIZE);
    }
}

void pmm_init(multiboot_info_t *mbd, uint32_t bitmap_addr) {
    uint32_t highest_addr = 0;
    multiboot_memory_map_t *mmap =
        (multiboot_memory_map_t *)(mbd->mmap_addr + 0xC0000000);

    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length + 0xC0000000) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            if (mmap->addr <= 0xFFFFFFFF) {
                uint32_t region_end;

                if ((uint64_t)(mmap->addr + mmap->len) > 0xFFFFFFFF) {
                    region_end = 0xFFFFF000;
                } else {
                    region_end = (uint32_t)(mmap->addr + mmap->len);
                }

                if (region_end > highest_addr) {
                    highest_addr = region_end;
                }
            }
        }
        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size +
                                          sizeof(mmap->size));
    }

    pmm.max_blocks = PAGE_ALIGN_UP(highest_addr) / 4096;
    pmm.used_blocks = pmm.max_blocks;
    pmm.array = (uint32_t *)bitmap_addr;
    pmm.last_allocated_bit = 0;

    // Mark everything as used by default
    uint32_t bitmap_size_bytes = ((pmm.max_blocks + 31) / 32) * 4;
    memset(pmm.array, 0xFF, bitmap_size_bytes);

    mmap = (multiboot_memory_map_t *)(mbd->mmap_addr + 0xC0000000);

    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length + 0xC0000000) {
        if (mmap->addr <= 0xFFFFFFFF &&
            mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t region_start = PAGE_ALIGN_UP(mmap->addr);
            uint32_t region_end;

            if ((uint64_t)(mmap->addr + mmap->len) > 0xFFFFFFFF) {
                region_end = 0xFFFFF000;
            } else {
                region_end = PAGE_ALIGN_DOWN(mmap->addr + mmap->len);
            }

            uint32_t start_frame = region_start / PAGE_SIZE;
            uint32_t end_frame = region_end / PAGE_SIZE;

            for (uint32_t i = start_frame; i < end_frame; i++) {
                if (bitmap_check(i)) {
                    bitmap_clear(i);
                    pmm.used_blocks--;
                }
            }
        }
        mmap = (multiboot_memory_map_t *)((uint32_t)mmap + mmap->size +
                                          sizeof(mmap->size));
    }

    // Protect kernel
    pmm_mark_region_used((uint32_t)&kernel_start - 0xC0000000,
                         (uint32_t)&kernel_end - 0xC0000000);

    // Protect bitmap
    pmm_mark_region_used(bitmap_addr - 0xC0000000,
                         bitmap_addr + bitmap_size_bytes - 0xC0000000);

    // Protect first 1MB (BIOS, VGA buffer, etc.)
    pmm_mark_region_used(0, 0x100000);
}

uint32_t pmm_alloc_frame() {
    for (uint32_t i = pmm.last_allocated_bit; i < pmm.max_blocks; i++) {
        if (!bitmap_check(i)) {
            pmm_mark_used(i * 4096);
            pmm.last_allocated_bit = i;
            return (uint32_t)i * 4096;
        }
    }

    for (uint32_t i = 0; i < pmm.last_allocated_bit; i++) {
        if (!bitmap_check(i)) {
            pmm_mark_used(i * 4096);
            pmm.last_allocated_bit = i;
            return (uint32_t)i * 4096;
        }
    }

    return 0;
}