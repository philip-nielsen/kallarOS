#include "src/gdt.h"
#include "src/idt.h"
#include "src/drivers/apic.h"
#include "src/io.h"
#include "src/drivers/vga.h"
#include "src/kernel/timer.h"
#include "src/kernel/multiboot.h"
#include "src/kernel/panic.h"
#include "src/kernel/pmm.h"

#include <stdint.h>

static uint32_t bitmap[4096];
int kmain(multiboot_info_t* mbd, uint32_t magic) {
    clear_screen();

    /* Make sure the magic number matches for memory mapping*/
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("invalid magic number!");
    }

    /* Check bit 6 to see if we have a valid memory map */
    if(!(mbd->flags >> 6 & 0x1)) {
        panic("invalid memory map given by GRUB bootloader");
    }

    kprintf("Init PMM\n");
    pmm_init(mbd, (uint32_t)bitmap);

    kprintf("Booting OS\n");
    initGdt();
    kprintf("GDT initialized\n");
    idt_init();
    kprintf("IDT initialized\n");
    outb(0x21, inb(0x21) | 0x01); // MASK LEGACY IRQ 0 (PIT)

    apic_start_timer();
    kprintf("APIC timer started\n");
    
    uint32_t uptime_seconds = 0;
    char spinner[] = {'|', '/', '-', '\\'};

    kprintf("System Uptime: \n");

    while (1) {
        // \r brings us back to the start of the line so we overwrite the old time
        // Using %c for the spinner char and %d for the integer
        kprintf("\r  [%c] %d seconds active...", spinner[uptime_seconds % 4], uptime_seconds);
        sleep_ms(1000); 
        uptime_seconds++;
    }
}