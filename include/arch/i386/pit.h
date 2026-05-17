#ifndef INCLUDE_PIT_H
#define INCLUDE_PIT_H

#include <stdint.h>

/**
 * pit_prepare_sleep() - Prepares the legacy PIT to sleep for the desired
 * microseconds.
 * @microseconds: How many microseconds the system is supposed to sleep.
 *
 * This is a calibration helper for the APIC. It is intended to be used only
 * once during boot to measure the APIC timer's tick rate.
 */
void pit_prepare_sleep(uint32_t microseconds);

/**
 * pit_perform_sleep() - Blocks execution until the PIT sleep completes.
 *
 * This function polls the PIT gate port, putting the CPU into a low-power
 * spinloop using the x86 `pause` instruction until the time elapses. It is
 * used during APIC initialization.
 */
void pit_perform_sleep(void);

#endif