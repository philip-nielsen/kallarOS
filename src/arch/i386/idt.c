#include <arch/i386/idt.h>
#include <arch/i386/io.h>
#include <arch/i386/pic.h>
#include <drivers/keyboard.h>
#include <kernel/panic.h>
#include <libc/stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define KEYBOARD_VECTOR 33
#define SPURIOUS_INTERRUPTS_VECTOR 39
#define SLAVE_INTERRUPTS_VECTOR 47
#define APIC_VECTOR 200

typedef struct {
    uint16_t isr_low;   // The lower 16 bits of the ISR's address
    uint16_t kernel_cs; // The GDT segment selector that the CPU will load into
                        // CS before calling the ISR
    uint8_t reserved;   // Set to zero
    uint8_t attributes; // Type and attributes; see the IDT page
    uint16_t isr_high;  // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

extern void irq_stub_1(void);
extern void isr_apic_timer(void);
extern void isr_spurious(void);

static bool vectors[256];

extern void *isr_stub_table[];

__attribute__((aligned(0x10))) static idt_entry_t
    idt[256]; // Create an array of IDT entries; aligned for performance
static idtr_t idtr;

void exception_handler(uint32_t interrupt_num, uint32_t error_code) {
    if (interrupt_num == 14) {
        uint32_t faulting_address;
        __asm__ volatile("mov %%cr2, %0" : "=r"(faulting_address));

        printf("Page Fault at Virtual Address: 0x%x\n", faulting_address);

        if ((error_code & 0x1) == 0) {
            printf("Page not present\n");
        } else {
            printf("Page protection violation\n");
        }

        if ((error_code & 0x2) == 2) {
            printf("Action: Write\n");
        } else {
            printf("Action: Read\n");
        }

        if ((error_code & 0x4) == 4) {
            printf("User Mode\n");
        } else {
            printf("Kernel Mode\n");
        }

    } else {
        printf("KERNEL PANIC! CPU EXCEPTION: %d\nERROR CODE: 0x%x\n",
               interrupt_num, error_code);
    }

    panic("Unhandled Hardware Exception!");
}

static void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // kernel cdoe selector
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void idt_init() {
    idtr.base = (uint32_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    pic_remap(0x20, 0x28); // Start Master interrupts at IDT index 32 (0x20),
                           // and Slave interrupts at IDT index 40 (0x28)

    idt_set_descriptor(
        APIC_VECTOR, isr_apic_timer,
        0x8E); // The Timer at index 200, as to not collide with legacy pics
    idt_set_descriptor(SPURIOUS_INTERRUPTS_VECTOR, isr_spurious,
                       0x8E); // The Spurious Interrupt
    idt_set_descriptor(KEYBOARD_VECTOR, irq_stub_1, 0x8E); // The keyboard
    idt_set_descriptor(SLAVE_INTERRUPTS_VECTOR, isr_spurious,
                       0x8E); // The Slave Spurious Interrupt

    __asm__ volatile("lidt %0" : : "m"(idtr)); // Load the new IDT

    inb(0x60);

    __asm__ volatile("sti"); // set the new interrupt flag
}

void irq_handler(uint32_t irq) {
    if (irq == 1) {
        keyboard_handler();
    }

    // Check if came from slave
    if (irq >= 8) {
        outb(0xA0, 0x20); // Send to slave
    }

    // Send to master
    outb(0x20, 0x20);
}