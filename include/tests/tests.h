#ifndef TESTS_H
#define TESTS_H

/**
 * kmalloc_run_test() - Runs some tests for the kernel heap.
 * Tests allocation, fragmentation and the first fit logic.
 * Context: Causes Kernel Panic if the tests fails.
 */
void kmalloc_run_test();

#endif