#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

/**
 * idt_init() - Initializes the Interrupt Descriptor Table (IDT).
 * Populates the x86 IDT with exception and hardware interrupt handlers.
 * This function also remaps the legacy PICs to avoid vector collisions
 * and globally enables interrupts (sti) before returning.
 */
void idt_init(void);

#endif