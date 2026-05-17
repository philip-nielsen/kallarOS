#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H
#include <stdint.h>

/**
 * sleep_ms() - Suspends CPU execution for the desired milliseconds.
 * @ms: The target number of milliseconds to sleep.
 *
 * Uses the system tick counter to halt the CPU (`hlt` instruction) in a
 * loop until the desired time has elapsed.
 *
 * Note: Because the underlying APIC timer ticks every 10ms, this function
 * has a 10ms granularity. The requested time will be rounded down to the
 * nearest 10ms interval (e.g., 15ms becomes 10ms).
 */
void sleep_ms(uint32_t ms);

#endif