#include "src/io.h"
#include "src/gdt.h"
#include "src/idt.h"


int kmain() {
    // char test[] = "Potatis";
    // write_chars(test, 50);
    initGdt();
    idt_init();
    while (1) {
        __asm__ volatile ("hlt"); // Puts the CPU to sleep until the next interrupt
    }
}   