#include <arch/i386/paging.h>
#include <kernel/kmalloc.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct block_header {
    uint32_t size_and_flags; // 31 bits for size, the lsb for used
    struct block_header *next_free_slot;
} block_header_t;

static block_header_t *free_list_head = 0;

static uint32_t heap_watermark = 0;

uint32_t malloc(size_t bytes_to_allocate) {
    if (bytes_to_allocate <= 0) {
        return 0x0;
    }

    uint32_t real_size =
        (bytes_to_allocate + sizeof(block_header_t) + 3) & ~0x3;

    block_header_t *current = free_list_head;
    block_header_t *previous = 0;

    while (current != 0) {
        uint32_t size = current->size_and_flags & ~0x1;

        if (size >= real_size) {

            uint32_t remainging_space = size - real_size;

            if (remainging_space >= sizeof(block_header_t) + 4) {
                block_header_t *new_block =
                    (block_header_t *)((uint32_t)current + real_size);
                new_block->size_and_flags = remainging_space;
                new_block->size_and_flags |= 0x1;

                current->size_and_flags = real_size;
                current->size_and_flags &= ~0x1;

                new_block->next_free_slot = current->next_free_slot;

                if (previous == 0) {
                    free_list_head = new_block;
                } else {
                    previous->next_free_slot = new_block;
                }

            } else {
                current->size_and_flags &= ~0x1;

                if (previous == 0) {
                    free_list_head = current->next_free_slot;
                } else {
                    previous->next_free_slot = current->next_free_slot;
                }
            }
            return (uint32_t)current + sizeof(block_header_t);
        }
        previous = current;
        current = current->next_free_slot;
    }

    uint32_t new_block_addr = heap_watermark;

    uint32_t pages_needed = (real_size + 4095) / 4096;
    for (uint32_t i = 0; i < pages_needed; i++) {
        alloc_page(heap_watermark, PTE_PRESENT | PTE_RW);
        heap_watermark += 4096;
    }

    block_header_t *new_block = (block_header_t *)new_block_addr;

    new_block->size_and_flags = (pages_needed * 4096);
    new_block->size_and_flags |= 0x1;
    new_block->next_free_slot = 0;

    if (free_list_head == 0) {
        free_list_head = new_block;
    } else {
        previous->next_free_slot = new_block;
    }

    return malloc(bytes_to_allocate);
}

void kmalloc_init() {
    extern uint32_t kernel_end;

    heap_watermark = (uint32_t)&kernel_end + 0x50000;
}