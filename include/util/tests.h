#ifndef TESTS_H
#define TESTS_H

/**
 * multitasking_run_test() - Runs some tests for multitasking.
 * Tests thread creating, mutexes and semaphores.
 */
void multitasking_run_test(void);

/**
 * kmalloc_run_test() - Runs some tests for the kernel heap.
 * Tests allocation, fragmentation and the first fit logic.
 * Context: Causes Kernel Panic if the tests fails.
 */
void kmalloc_run_test(void);

/**
 * ata_test() - Runs a write read loopback test.
 * Context: Causes Kernel Panic if the tests fails.
 */
void ata_test(void);

#endif