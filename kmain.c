#include "io/write.c"

int sum_of_three(int arg1, int arg2, int arg3) {
    return arg1 + arg2 + arg3;
}

int main() {
    fb_write_cell(0, 'A', FB_GREEN, FB_DARK_GREY);
}