#include "io/io.h"


int kmain() {
    // fb_write_cell(0, 'Q', FB_GREEN, FB_DARK_GREY);
    // fb_move_cursor(1);
    // char* test = malloc(10 * sizeof(char));
    char test[] = "P";
    write_chars(test, 100);
    return 0;
}