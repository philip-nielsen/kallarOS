#include <arch/i386/apic.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/io.h>
#include <arch/i386/multiboot.h>
#include <arch/i386/paging.h>
#include <drivers/ata.h>
#include <drivers/vga.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/pmm.h>
#include <kernel/scheduler.h>
#include <kernel/thread.h>
#include <kernel/timer.h>
#include <libc/stdio.h>
#include <util/tests.h>

#include <stdint.h>

extern uint32_t kernel_end;

int kmain(multiboot_info_t *mbd, uint32_t magic) {
    vga_clear_screen();
    printf("Booting OS\n");
    initGdt();
    printf("GDT initialized\n");
    idt_init();
    printf("IDT initialized\n");

    mbd = (multiboot_info_t *)((uint32_t)mbd + 0xC0000000);

    /* Make sure the magic number matches for memory mapping*/
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("invalid magic number!");
    }

    /* Check bit 6 to see if we have a valid memory map */
    if (!(mbd->flags >> 6 & 0x1)) {
        panic("invalid memory map given by GRUB bootloader");
    }

    uint32_t bitmap_addr = (((uint32_t)&kernel_end) + 0xFFF) & ~0xFFF;

    printf("Init PMM\n");
    pmm_init(mbd, bitmap_addr);

    printf("Init Paging\n");
    paging_init();

    outb(0x21, inb(0x21) | 0x01); // MASK LEGACY IRQ 0 (PIT)
    map_page(0xFEE00000, 0xFEE00000,
             PTE_PRESENT | PTE_RW); // Identity map the APIC time

    apic_start_timer();
    printf("APIC timer started\n");

    printf("Checking kernel heap\n");
    kmalloc_init();
    kmalloc_run_test();

    initialize_multitasking();
    printf("Multitasking initialized\n");

    printf("Checking multitasking\n");
    multitasking_run_test();

    uint32_t uptime_seconds = 0;
    char spinner[] = {'|', '/', '-', '\\'};

    printf("\n");
    identify_disk();
    printf("\n");

    printf("System Uptime: \n");

    while (1) {
        printf("\r  [%c] %d seconds active...", spinner[uptime_seconds % 4],
               uptime_seconds);
        sleep_ms(1000);
        uptime_seconds++;
    }
}