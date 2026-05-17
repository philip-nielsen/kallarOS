#ifndef STRING_H
#define STRING_H

#include <stddef.h>

/**
 * memcpy() - Copies bytes from one memory area to another.
 * @dst: Pointer to the destination array.
 * @src: Pointer to the source data.
 * @n: Number of bytes to copy.
 *
 * Return: A pointer to the destination (dst).
 */
void *memcpy(void *restrict dst, const void *restrict src, size_t n);

/**
 * memset() - Fills a block of memory with a specific byte.
 * @ptr: Pointer to the block of memory to fill.
 * @x: The value to be set (passed as an int, cast to unsigned char).
 * @n: Number of bytes to be set to the value.
 *
 * Return: A pointer to the memory area (ptr).
 */
void *memset(void *ptr, int x, size_t n);

/**
 * strlen() - Calculates the length of a string.
 * @str: The null-terminated string to measure.
 *
 * Return: The length of the string, excluding the null byte.
 */
size_t strlen(const char *str);

/**
 * strcmp() - Compares two strings.
 * @str1: The first string to compare.
 * @str2: The second string to compare.
 *
 * Return: 0 if the strings are exactly equal, -1 if str1 is less than str2,
 * or 1 if str1 is greater than str2. Returns -1 on null pointers.
 */
int strcmp(const char *str1, const char *str2);

#endif