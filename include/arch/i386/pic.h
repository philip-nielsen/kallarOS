#ifndef INCLUDE_PIC_H
#define INCLUDE_PIC_H

#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

/**
 * pic_remap() - Remaps the legacy Programmable Interrupt Controllers (PICs).
 * @offset1: The vector offset for the Master PIC (becomes offset1 to
 * offset1+7).
 * @offset2: The vector offset for the Slave PIC (becomes offset2 to offset2+7).
 *
 * By default, PIC interrupts conflict with x86 CPU exceptions. This function
 * sends the initialization ICW commands to remap the Master and Slave IRQs to
 * safe vectors in the IDT.
 */
void pic_remap(uint8_t offset1, uint8_t offset2);

#endif