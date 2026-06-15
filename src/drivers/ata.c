#define PRINT 1
#include <arch/i386/io.h>
#include <drivers/ata.h>
#include <kernel/panic.h>
#include <util/debug.h>

#include <stdint.h>

void identify_disk() {
    outb(0x1F6, 0xA0);
    for (int address = 0x1F2; address < 0x1F6; address++) {
        outb(address, 0);
    }

    outb(0x1F7, 0xEC);

    if (!inb(0x1F7)) {
        panic("no driver!\n");
    }

    while (inb(0x1F7) & 0x80) {
    }

    if (inb(0x1F7) & 0x01) {
        panic("abort!\nError on ATA");
    }

    if (inb(0x1F7) & 0x08) {
        uint16_t read_buffer[256];

        for (int i = 0; i < 256; i++) {
            read_buffer[i] = inw(0x1F0);
        }
        uint32_t total_size =
            ((uint32_t)read_buffer[60] | (uint32_t)read_buffer[61] << 16) /
            2048;
        pr("Total disk size is %d", total_size);
    }
}