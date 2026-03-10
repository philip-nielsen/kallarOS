#include <stdint.h>
#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_GREEN 2
#define FB_DARK_GREY 8

//Sends the given data to the given I/O port
extern void outb(unsigned short port, unsigned char data);

// Writes a character with the given foreground and background to position i
// in the framebuffer.
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

//Moves the cursor of the framebuffer to the given position
void fb_move_cursor(unsigned short pos);

//Write the array of chars to the framebuffer at the given postion, and moves the cursor to the end of the chars.
void write_chars(char chars[], unsigned int pos);

//Reads a byte from the given I/O port.
uint8_t inb(unsigned short port);

#endif