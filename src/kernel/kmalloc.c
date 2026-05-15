#include <arch/i386/paging.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <libc/stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct current {
    uint32_t size_and_flags; // 31 bits for size, the lsb for used
    struct current *next_free_slot;
} block_header_t;

static block_header_t *free_list_head = 0;

extern uint32_t kernel_end;
static uint32_t heap_watermark = (uint32_t)&kernel_end + 0x50000;

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

            uint32_t remaining_space = size - real_size;

            if (remaining_space >= sizeof(block_header_t) + 4) {
                block_header_t *new_block =
                    (block_header_t *)((uint32_t)current + real_size);
                new_block->size_and_flags = remaining_space;
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

    uint32_t remaining_space = pages_needed - real_size;
    block_header_t *new_block = (block_header_t *)new_block_addr;

    if (remaining_space >= sizeof(block_header_t) + 4) {
        block_header_t *last_block =
            (block_header_t *)(new_block_addr + real_size);
        last_block->size_and_flags = remaining_space;
        last_block->size_and_flags |= 0x1;
        last_block->next_free_slot = 0;

        new_block->size_and_flags = real_size;
        new_block->size_and_flags &= ~0x1;

        if (free_list_head == 0) {
            free_list_head = last_block;
        } else {
            previous->next_free_slot = last_block;
        }
    } else {
        new_block->size_and_flags = pages_needed & ~0x1;
    }

    return new_block_addr + sizeof(block_header_t);
}

void kfree(uint32_t address) {
    block_header_t *current =
        (block_header_t *)(address - sizeof(block_header_t));

    if (current->size_and_flags & 0x1) {
        printf("Use after free for %d\n", address);
        panic("USE AFTER FREE!");
    }

    current->size_and_flags |= 0x1;

    block_header_t *head = free_list_head;
    block_header_t *prev = 0;

    while (head != 0 && (uint32_t)head < (uint32_t)current) {
        prev = head;
        head = head->next_free_slot;
    }

    current->next_free_slot = head;
    if (prev == 0) {
        free_list_head = current;
    } else {
        prev->next_free_slot = current;
    }

    uint32_t current_size = current->size_and_flags & ~0x1;

    if (head != 0) {
        if ((uint32_t)current + current_size ==
            (uint32_t)head->next_free_slot) {
            current->size_and_flags +=
                (head->next_free_slot->size_and_flags & ~0x1);
            current->next_free_slot = head->next_free_slot->next_free_slot;
        }
    }

    if (prev != 0) {
        uint32_t prev_size = prev->size_and_flags & ~0x1;
        if ((uint32_t)prev + prev_size == (uint32_t)current) {
            prev->size_and_flags += (current_size);
            prev->next_free_slot = current->next_free_slot;
        }
    }
}