#include "vga.h"
#include "../io.h"

static char *fb = (char *) 0x000B8000;
static unsigned int cursor_pos = 0;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i] = c;
    fb[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
}  

void write_chars(const char* chars, unsigned int pos) {
    int len = 0;
    while (chars[len]) {
        fb_write_cell(pos + (len * 2), chars[len], 0, 0x0F);
        len += 1;
    } 
    fb_move_cursor(pos / 2 + len);
}

void print(const char* chars) {
    int i = 0;
    while (chars[i] != '\0') {
        if (chars[i] == '\n') {
            // Move cursor to the start of the next line.
            cursor_pos = (cursor_pos / 160 + 1) * 160;
        } else if (chars[i] == '\r') {
            cursor_pos = (cursor_pos / 160) * 160;
        } else {
            fb_write_cell(cursor_pos, chars[i], FB_WHITE, FB_BLACK);
            cursor_pos += 2;
        }

        i++;

        if (cursor_pos >= 4000) { //Loop back until scrolling is implemted
            cursor_pos = 0; 
        }

        fb_move_cursor(cursor_pos / 2);
    }
}

void print_int(int n) {
    if (n == 0) {
        print("0");
        return;
    }

    char buffer[12]; //Max length of a 32-bit int + sign + null terminator
    int i = 0;
    int is_negative = 0;

    unsigned int num = n;
    if (n < 0) {
        is_negative = 1;
        num = -n;
    }

    while (num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // Reverse back number string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--; 
    }

    print(buffer);
}

void print_color(const char* chars, vga_color_t color) {
    int i = 0;
    while (chars[i] != '\0') {
        if (chars[i] == '\n') {
            // Move cursor to the start of the next line.
            cursor_pos = (cursor_pos / 160 + 1) * 160;
        } else if (chars[i] == '\r') {
            cursor_pos = (cursor_pos / 160) * 160;
        } else {
            fb_write_cell(cursor_pos, chars[i], color, FB_BLACK);
            cursor_pos += 2;
        }

        i++;

        if (cursor_pos >= 4000) { //Loop back until scrolling is implemted
            cursor_pos = 0; 
        }

        fb_move_cursor(cursor_pos / 2);
    }
}

void clear_screen() {
    for (int i = 0; i < 80 * 25; i++) {
        fb_write_cell(i * 2, ' ', FB_WHITE, FB_BLACK);
    }
    cursor_pos = 0;
    
    fb_move_cursor(0);
}