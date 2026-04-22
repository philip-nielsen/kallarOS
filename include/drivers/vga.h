#ifndef INCLUDE_VGA_H
#define INCLUDE_VGA_H

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

typedef enum {
    FB_BLACK = 0,
    FB_BLUE = 1,
    FB_GREEN = 2,
    FB_CYAN = 3,
    FB_RED = 4,
    FB_MAGENTA = 5,
    FB_BROWN = 6,
    FB_LIGHT_GREY = 7,
    FB_DARK_GREY = 8,
    FB_LIGHT_BLUE = 9,
    FB_LIGHT_GREEN = 10,
    FB_LIGHT_CYAN = 11,
    FB_LIGHT_RED = 12,
    FB_LIGHT_MAGENTA = 13,
    FB_YELLOW = 14,
    FB_WHITE = 15
} vga_color_t;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);
void fb_move_cursor(unsigned short pos);
void write_chars(const char* chars, unsigned int pos);

void print(const char* chars);
void print_int(int n);
void print_color(const char* chars, vga_color_t color);
void clear_screen(void);
void print_char(char c);
void print_hex(unsigned int n);
void kprintf(const char* format, ...);

#endif