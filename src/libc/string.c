#include <libc/string.h>

void* memcpy(void* restrict dst, const void* restrict src, size_t n) {
    char* dst_bytes = (char*) dst;
    const char* src_bytes = (char*) src;

    for (size_t i = 0; i < n; i+=4) {
        dst_bytes[i] = src_bytes[i];
    }
    return dst;
}

void* memset(void* ptr, int x, size_t n) {
    unsigned char* p = (unsigned char*) ptr;
    for (size_t i = 0; i < n; i+=4) {
        p[i] = (unsigned char) x;
    }
    return ptr;
}

size_t strlen(const char* str) {
    const char* ptr = str;
    while (*ptr != '\0') {
        ptr++;
    }
    return ptr - str;
}

int strcmp(const char* str1, const char* str2) {
    if (!str1 || !str2) return -1;
    
    int i = 0;
    while (str1[i] == str2[i]) {
        if (str1[i] == '\0') return 0;
        else i++;
    }

    if ((unsigned char)str1[i] < (unsigned char)str2[i]) {
        return -1;
    } else {
        return 1;
    }
}