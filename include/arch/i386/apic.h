#ifndef INCLUDE_APIC_H
#define INCLUDE_APIC_H

#include <stdint.h>

/**
 * apic_start_timer() - Initializes the Local APIC timer.
 * Sets up the APIC timer hardware to fire an interrupt every 10ms.
 */
void apic_start_timer(void);

/**
 * apic_timer_handler() - Acknowledges the timer interrupt.
 * Called during the APIC timer interrupt. Updates the internal
 * system_ticks counter by one.
 */
void apic_timer_handler(void);

/**
 * apic_read() - Reads a value from a Local APIC register.
 * @reg: The memory offset of the Local APIC register to read.
 *
 * Return: The 32-bit value currently held in the register.
 */
uint32_t apic_read(uint32_t reg);

/**
 * apic_write() - Writes a value to a Local APIC register.
 * @reg: The memory offset of the Local APIC register to write to.
 * @value: The 32-bit value to write into the register.
 */
void apic_write(uint32_t reg, uint32_t value);

/**
 * apic_get_ticks() - Retrieves the system uptime in ticks.
 *
 * Return: The total number of 10ms ticks that have occurred since
 * the timer was started.
 */
uint64_t apic_get_ticks(void);

#endif