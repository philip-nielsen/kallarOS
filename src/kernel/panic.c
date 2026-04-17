#include "panic.h"

#include "../drivers/vga.h" 

void panic(const char* message) {
    print_color("\n========================================\n", FB_RED);
    print_color("              KERNEL PANIC              \n", FB_RED);
    print_color("========================================\n", FB_RED);
    
    print_color("FATAL ERROR: ", FB_RED);
    print_color(message, FB_RED);
    print_color("\n\nSystem halted. Please restart.", FB_RED);

    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}