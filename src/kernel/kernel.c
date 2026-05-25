#include <arch/i386/apic.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/io.h>
#include <arch/i386/multiboot.h>
#include <arch/i386/paging.h>
#include <drivers/vga.h>
#include <kernel/kmalloc.h>
#include <kernel/panic.h>
#include <kernel/pmm.h>
#include <kernel/task.h>
#include <kernel/timer.h>
#include <libc/stdio.h>
#include <tests/tests.h>

#include <stdint.h>

extern uint32_t kernel_end;

void task_a() {
    for (;;) {
        printf("A");
        yield();
    }
}

void task_b() {
    for (;;) {
        printf("B");
        yield();
    }
}

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

    thread_control_block_t *thread_a = create_kernel_task(task_a);
    thread_control_block_t *thread_b = create_kernel_task(task_b);
    thread_control_block_t *current_thread = get_current_thread();
    thread_a->id = 1;
    thread_b->id = 2;

    current_thread->next = thread_a;
    thread_a->next = thread_b;
    thread_b->next = current_thread;

    for (;;) {
        printf("M");
        yield();
    }

    // uint32_t uptime_seconds = 0;
    // char spinner[] = {'|', '/', '-', '\\'};

    // printf("System Uptime: \n");

    // while (1) {
    //     printf("\r  [%c] %d seconds active...", spinner[uptime_seconds % 4],
    //            uptime_seconds);
    //     sleep_ms(1000);
    //     uptime_seconds++;
    // }
}