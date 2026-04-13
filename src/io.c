#include "io.h"

void outb(uint16_t port, uint8_t data) {
    __asm__ volatile (
        "outb %1, %0"
        :
        : "dN"(port), "a"(data)
    );
}

uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );
    return result;
}