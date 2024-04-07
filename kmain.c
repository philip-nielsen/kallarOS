// #include "io/io.c"
#include "io/write.c"


int main() {
    fb_write_cell(0, 'Q', FB_GREEN, FB_DARK_GREY);
    // outb_vga('A', 0x3D4); // Example: send 'A' to VGA port 0x3D4
    //fb_move_cursor(1);
    *(char*)0xb8000 = 'Q';
    return 0;
}