#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include <stdint.h>

// Sends the given data to the given I/O port
void outb(uint16_t port, uint8_t data);

// Reads a byte from the given I/O port
uint8_t inb(uint16_t port);

#endif