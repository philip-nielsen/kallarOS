#include <drivers/vga.h>
#include <arch/i386/io.h>

static char *fb = (char *) 0x000B8000 + 0xC0000000;
static uint32_t cursor_pos = 0;

static uint8_t current_color = FB_WHITE | (FB_BLACK << 4);

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    current_color = fg | (bg << 4);
}

void fb_write_cell(uint32_t i, char c, uint8_t fg, uint8_t bg) {
    fb[i] = c;
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_move_cursor(uint16_t pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}  

static void scroll() {
    for (int i = 0; i < 3840; i++) {
        fb[i] = fb[i + 160];
    }

    for (int i = 3840; i < 4000; i += 2) {
        fb[i] = ' ';
        fb[i + 1] = 0x0F;
    }

    cursor_pos = 3840; 
}

void vga_putchar(char c) {
    if (c == '\n') {
        cursor_pos = (cursor_pos / 160 + 1) * 160;
    } else if (c == '\r') {
        cursor_pos = (cursor_pos / 160) * 160;
    } else {
        fb[cursor_pos] = c;
        fb[cursor_pos + 1] = current_color;
        cursor_pos += 2;
    }

    if (cursor_pos >= 4000) scroll(); 
    fb_move_cursor(cursor_pos / 2);
}

void vga_clear_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        fb_write_cell(i * 2, ' ', FB_WHITE, FB_BLACK);
    }
    cursor_pos = 0;
    
    fb_move_cursor(0);
}