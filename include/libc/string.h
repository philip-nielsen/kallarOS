#ifndef STRING_H
#define STRING_H
#include <stddef.h>

void* memcpy(void* restrict dst, const void* restrict src, size_t n);
void* memset(void* ptr, int x, size_t n);
size_t strlen(const char* str);
int strcmp(const char* str1, const char* str2);

#endif