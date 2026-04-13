#include "vga.h"
#include "../io.h" // Include our newly separated I/O ports!

static char *fb = (char *) 0x000B8000;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}  

void write_chars(char chars[], unsigned int pos) {
    int len = 0;
    while (chars[len]) {
        fb_write_cell(pos + (len * 2), chars[len], 0, 0x0F);
        len += 1;
    } 
    fb_move_cursor(pos / 2 + len);
}