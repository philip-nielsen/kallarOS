#ifndef INCLUDE_VGA_H
#define INCLUDE_VGA_H

#include <stdint.h>

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

/**
 * fb_write_cell() - Writes a character and its color to the framebuffer.
 * @i: The byte offset index in the framebuffer array.
 * @c: The ASCII character to write.
 * @fg: The foreground color.
 * @bg: The background color.
 *
 * Writes the character byte followed by the combined color attribute byte
 * directly to the VGA memory mapping.
 */
void fb_write_cell(uint32_t i, char c, uint8_t fg, uint8_t bg);

/**
 * fb_move_cursor() - Moves the hardware blinking cursor to the desired
 * position.
 * @pos: The character cell index (0 to 1999) to move the cursor to.
 */
void fb_move_cursor(uint16_t pos);

/**
 * vga_clear_screen() - Clears the entire VGA text-mode screen.
 * Overwrites the entire framebuffer with whitespace, sets the background
 * to black, and resets the hardware cursor to the top-left (position 0).
 */
void vga_clear_screen(void);

/**
 * vga_putchar() - Prints a character to the screen at the current cursor
 * position.
 * @c: Character to write to the framebuffer.
 *
 * Supports special characters like '\n' and '\r'. Automatically handles
 * advancing the cursor and scrolling the terminal if the bottom of the
 * screen is reached.
 */
void vga_putchar(char c);

/**
 * vga_set_color() - Sets the active global foreground and background colors.
 * @fg: Foreground color to set.
 * @bg: Background color to set.
 * Affects all subsequent calls to vga_putchar().
 */
void vga_set_color(vga_color_t fg, vga_color_t bg);

#endif