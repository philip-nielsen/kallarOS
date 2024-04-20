#include "io.h"

void fb_move_cursor(unsigned short pos) {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}  

static char *fb = (char *) 0x000B8000;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
  fb[i] = c;
  fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void outb(unsigned short port, unsigned char data) {
  __asm__(
    mov al, [esp + 8]    ; Move the data byte from the stack to AL register
    mov dx, [esp + 4]    ; Move the address of the I/O port from the stack to DX register
    out dx, al           ; Send the data byte to the I/O port specified by DX
    ret                  ; Return from the function
  )
}
