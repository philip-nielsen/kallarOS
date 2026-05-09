#include <kernel/panic.h>
#include <libc/stdio.h>
#include <drivers/vga.h>

void panic(const char* message) {
    vga_set_color(FB_RED, FB_BLACK);
    printf("\n========================================\n");
    printf("              KERNEL PANIC              \n");
    printf("========================================\n");
    
    printf("FATAL ERROR: ");
    printf(message);
    printf("\n\nSystem halted. Please restart.");

    while (1) {
        __asm__ volatile ("cli; hlt");
    }
}