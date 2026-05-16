#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <libc/stdio.h>
#include <stdint.h>
#include <tests/tests.h>

void kmalloc_run_test() {
    uint32_t address1 = kmalloc(1000);
    uint32_t address2 = kmalloc(1000);
    uint32_t address3 = kmalloc(1000);

    kfree(address2);

    uint32_t address4 = kmalloc(400);
    uint32_t address5 = kmalloc(601);
    uint32_t address6 = kmalloc(201);

    if (address4 > address5 || address6 > address5) {
        panic("Invalid kernel fragmentation\n");
    }

    kfree(address4);
    kfree(address6);

    uint32_t address7 = kmalloc(505);

    if (address7 != address4) {
        panic("Invalid kernel fragmentation\n");
    }

    kfree(address1);
    kfree(address3);
    kfree(address5);
    kfree(address7);

    uint32_t zero_ptr = kmalloc(0);
    if (zero_ptr != 0) {
        panic("kmalloc(0) failed to return NULL!\n");
    }

    uint32_t exact_ptr1 = kmalloc(1024);
    kfree(exact_ptr1);

    uint32_t exact_ptr2 = kmalloc(1016);

    if (exact_ptr1 != exact_ptr2) {
        panic("Exact fit logic failed to reuse block!\n");
    }
    kfree(exact_ptr2);

    uint32_t massive_ptr = kmalloc(150000);
    if (massive_ptr == 0) {
        panic("Multi-page heap expansion failed!\n");
    }

    uint8_t *byte_ptr = (uint8_t *)massive_ptr;
    byte_ptr[149999] = 0xAA;

    kfree(massive_ptr);

    printf("All Heap Tests Passed!\n");
}