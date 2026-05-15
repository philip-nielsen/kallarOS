#include <arch/i386/apic.h>
#include <kernel/timer.h>

void sleep_ms(uint32_t ms) {
    // 1 tick = 10ms
    uint64_t target_ticks = (ms / 10);
    uint64_t end_time = apic_get_ticks() + target_ticks;

    while (apic_get_ticks() < end_time) {
        __asm__ volatile("hlt");
    }
}