#define PRINT 1
#include <arch/i386/io.h>
#include <drivers/ata.h>
#include <kernel/panic.h>
#include <util/debug.h>

#include <stdint.h>

void identify_disk(uint16_t *identify_buffer) {
    outb(0x1F6, 0xA0);
    for (int address = 0x1F2; address < 0x1F6; address++) {
        outb(address, 0);
    }

    outb(0x1F7, 0xEC);

    if (!inb(0x1F7)) {
        panic("no driver!\n");
    }

    while (inb(0x1F7) & 0x80)
        ;

    if (inb(0x1F7) & 0x01) {
        panic("abort!\nError on ATA");
    }

    while (!(inb(0x1F7) & 0x08))
        ;

    for (int i = 0; i < 256; i++) {
        identify_buffer[i] = inw(0x1F0);
    }

    pr("Total disk size is %d\n",
       ((uint32_t)identify_buffer[60] | (uint32_t)identify_buffer[61] << 16) /
           2048);

    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
    inb(0x3F6);
}

void read_sectors(uint32_t LBA, uint8_t sector_count, uint16_t *buffer) {
    outb(0x1F6, 0xE0 | (0 << 4) | ((LBA >> 24) & 0x0F));
    outb(0x1F1, 0x00);
    outb(0x1F2, (unsigned char)sector_count);
    outb(0x1F3, (unsigned char)LBA);
    outb(0x1F4, (unsigned char)(LBA >> 8));
    outb(0x1F5, (unsigned char)(LBA >> 16));
    outb(0x1F7, 0x20);

    uint16_t sector_counts = sector_count == 0 ? 256 : sector_count;
    for (int i = 0; i < sector_counts; i++) {
        while (inb(0x1F7) & 0x80)
            ;

        if (inb(0x1F7) & 0x01) {
            panic("abort!\nError on ATA");
        }

        if (inb(0x1F7) & 0x08) {
            for (int j = 0; j < 256; j++) {
                buffer[(i * 256) + j] = inw(0x1F0);
            }
        }
        inb(0x3F6);
        inb(0x3F6);
        inb(0x3F6);
        inb(0x3F6);
    }
}

void write_sectors(uint32_t LBA, uint8_t sector_count, uint16_t *buffer) {
    outb(0x1F6, 0xE0 | (0 << 4) | ((LBA >> 24) & 0x0F));
    outb(0x1F1, 0x00);
    outb(0x1F2, (unsigned char)sector_count);
    outb(0x1F3, (unsigned char)LBA);
    outb(0x1F4, (unsigned char)(LBA >> 8));
    outb(0x1F5, (unsigned char)(LBA >> 16));
    outb(0x1F7, 0x30);

    uint16_t sector_counts = sector_count == 0 ? 256 : sector_count;
    for (int i = 0; i < sector_counts; i++) {
        while (inb(0x1F7) & 0x80)
            ;

        if (inb(0x1F7) & 0x01) {
            panic("abort!\nError on ATA");
        }

        if (inb(0x1F7) & 0x08) {
            for (int j = 0; j < 256; j++) {
                outw(0x1F0, buffer[(i * 256) + j]);
            }
        }
        inb(0x3F6);
        inb(0x3F6);
        inb(0x3F6);
        inb(0x3F6);
    }
    flush_cache();
}

void flush_cache(void) {
    outb(0x1F7, 0xE7);
    while (inb(0x1F7) & 0x80)
        ;
}