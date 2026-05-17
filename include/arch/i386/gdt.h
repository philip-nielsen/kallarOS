#ifndef INCLUDE_GDT_H
#define INCLUDE_GDT_H

#include <stdint.h>

/**
 * initGdt() - Initializes the GDT.
 * Sets up all the correct segments for the x86 global descriptor table.
 */
void initGdt(void);

/**
 * setGdtGate() - Sets up a descriptor in the Global Descriptor Table.
 * @num:    The index in the GDT array to configure (e.g., 1 for Kernel Code).
 * @base:   The 32-bit linear base address of the segment.
 * @limit:  The maximum addressable unit for the segment.
 * @access: The access byte, determining ring level and executable/read/write
 * permissions.
 * @gran:   The granularity byte, specifying 16/32-bit mode and limit scaling.
 *
 * Populates a specific entry in the internal gdt_entries array using standard
 * x86 segment descriptor formatting.
 */
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access,
                uint8_t gran);

#endif