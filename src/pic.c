#include "io.h"
#include "stdint.h"
#include "pic.h"

void pic_remap(int offset1, int offset2) {
    uint8_t a1, a2;

    // Save current masks
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // Start the initialization sequence in cascade mode
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);

    // Set the vector offsets
    outb(PIC1_DATA, offset1); // Master PIC vector offset
    outb(PIC2_DATA, offset2); //Slave PIC offset

    outb(PIC1_DATA, 4); // Slave PIC at IRQ2
    outb(PIC2_DATA, 2); // Cascade identity

    // Set mode to x86 mode
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    // Unmasking keyboard
    outb(0x21, 0xFD); 
    outb(0xA1, 0xFF);
}