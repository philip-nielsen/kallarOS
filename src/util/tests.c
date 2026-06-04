#define PRINT 0

#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <libc/stdio.h>
#include <util/debug.h>
#include <util/tests.h>

#include <stdint.h>

mutex_t *vga_mutex;
semaphore_t *bouncer_sem;
semaphore_t *test_barrier_sem;

void task_a() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_mutex(vga_mutex);
        for (int j = 0; j < 10; j++) {
            pr("A");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
        }
        thread_sleep(2);
        release_mutex(vga_mutex);
    }
    release_semaphore(test_barrier_sem);
}

void task_b() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_mutex(vga_mutex);
        for (int j = 0; j < 10; j++) {
            pr("B");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
            __asm__ volatile("nop");
        }
        thread_sleep(1);
        release_mutex(vga_mutex);
    }
    release_semaphore(test_barrier_sem);
}

void task_1() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_semaphore(bouncer_sem);
        pr("\n1\n");
        thread_sleep(2);
        release_semaphore(bouncer_sem);
    }
    release_semaphore(test_barrier_sem);
}

void task_2() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_semaphore(bouncer_sem);
        pr("2\n");
        thread_sleep(2);
        release_semaphore(bouncer_sem);
    }
    release_semaphore(test_barrier_sem);
}

void task_3() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_semaphore(bouncer_sem);
        pr("\n3\n");
        thread_sleep(2);
        release_semaphore(bouncer_sem);
    }
    release_semaphore(test_barrier_sem);
}

void task_4() {
    unlock_scheduler();
    for (int i = 0; i < 2; i++) {
        acquire_semaphore(bouncer_sem);
        pr("4\n");
        thread_sleep(2);
        release_semaphore(bouncer_sem);
    }
    release_semaphore(test_barrier_sem);
}

void multitasking_run_test() {
    vga_mutex = create_mutex();
    bouncer_sem = create_semaphore(2);
    test_barrier_sem = create_semaphore(6);

    for (int i = 0; i < 6; i++) {
        acquire_semaphore(test_barrier_sem);
    }

    create_kernel_thread(task_1);
    create_kernel_thread(task_2);
    create_kernel_thread(task_3);
    create_kernel_thread(task_4);

    create_kernel_thread(task_a);
    create_kernel_thread(task_b);

    for (int i = 0; i < 6; i++) {
        acquire_semaphore(test_barrier_sem);
    }

    kfree((uint32_t)vga_mutex);
    kfree((uint32_t)bouncer_sem);
    kfree((uint32_t)test_barrier_sem);

    printf("Multitasking tests passed!\n");
}

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