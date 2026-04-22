#ifndef INCLUDE_APIC_H
#define INCLUDE_APIC_H

#include <stdint.h>

// Initializes the Local APIC timer to tick every 10ms
void apic_start_timer(void);

// The C handler called by your assembly ISR wrapper
void apic_timer_handler(void);

// Reads a value from a Local APIC register
uint32_t apic_read(uint32_t reg);

// Writes a value to a Local APIC register
void apic_write(uint32_t reg, uint32_t value);

// Returns the number of 10ms ticks since the timer started
uint64_t apic_get_ticks(void);

#endif