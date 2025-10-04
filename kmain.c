#include "io/io.h"
#include "gdt/gdt.h"


int kmain() {
    // fb_write_cell(0, 'Q', FB_GREEN, FB_DARK_GREY);
    // fb_move_cursor(1);
    // char* test = malloc(10 * sizeof(char));
    char test[] = "Potatis";
    // char te[] = "A";
    write_chars(test, 50);
    initGdt();
    return 0;
}   