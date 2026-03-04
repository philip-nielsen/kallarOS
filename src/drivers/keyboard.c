#include "keyboard.h"
#include "../io.h"

const char kbd_US[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',  0,  ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0, '-'
};

static unsigned int current_cursor_loc = 160;

void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        char c = kbd_US[scancode];

        if (c != 0) {
            fb_write_cell(current_cursor_loc, c, 0, 0x0F);
            current_cursor_loc += 2;
            fb_move_cursor(current_cursor_loc / 2);
        }
    }
}