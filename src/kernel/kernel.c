#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/apic.h>
#include <arch/i386/io.h>
#include <arch/i386/multiboot.h>
#include <drivers/vga.h>
#include <kernel/timer.h>
#include <kernel/panic.h>
#include <kernel/pmm.h>

#include <stdint.h>

extern uint32_t kernel_end;

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

    uint32_t bitmap_addr = (((uint32_t)&kernel_end) + 0xFFF) & ~0xFFF;

    kprintf("Init PMM\n");
    pmm_init(mbd, bitmap_addr);

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
        kprintf("\r  [%c] %d seconds active...", spinner[uptime_seconds % 4], uptime_seconds);
        sleep_ms(1000); 
        uptime_seconds++;
    }
}