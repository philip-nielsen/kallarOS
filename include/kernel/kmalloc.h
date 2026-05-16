#ifndef KMALLOC_H
#define KMALLOC_H
#include <stddef.h>
#include <stdint.h>

uint32_t kmalloc(size_t bytes);
void kfree(uint32_t address);
void kmalloc_init();

#endif