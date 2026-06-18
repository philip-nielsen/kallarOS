#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include <stdint.h>

/**
 * outb() - Sends the given data to the given I/O port.
 * @port: The 16-bit I/O port to write to.
 * @data: The 8-bit value to send to the port.
 */
void outb(uint16_t port, uint8_t data);

/**
 * inb() - Reads the current data from the given I/O port.
 * @port: The 16-bit I/O port to read from.
 *
 * Return: The 8-bit value read from the specified port.
 */
uint8_t inb(uint16_t port);

/**
 * inw() - Reads the current data from the given I/O port.
 * @port: The 16-bit I/O port to read from.
 *
 * Return: The 16-bit value read from the specified port.
 */
uint16_t inw(uint16_t port);

/**
 * outw() - Sends the given data to the given I/O port.
 * @port: The 16-bit I/O port to write to.
 * @data: The 16-bit value to send to the port.
 */
void outw(uint16_t port, uint16_t data);

#endif