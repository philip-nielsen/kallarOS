#ifndef KMALLOC_H
#define KMALLOC_H
#include <stddef.h>
#include <stdint.h>

uint32_t malloc(size_t bytes);
void free(uint32_t address);
void kmalloc_init();

#endif