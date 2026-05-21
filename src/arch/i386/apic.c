#include <arch/i386/apic.h>
#include <arch/i386/pit.h>
#include <stdint.h>

// Tthe memory address of the APIC's End Of Interrupt (EOI) register
#define APIC_EOI_REGISTER 0xFEE000B0

// APIC Register Offsets
#define APIC_REGISTER_LVT_TIMER 0x320
#define APIC_REGISTER_TIMER_INITCNT 0x380
#define APIC_REGISTER_TIMER_CURRCNT 0x390
#define APIC_REGISTER_TIMER_DIV 0x3E0
#define APIC_REGISTER_SPURIOUS 0x0F0
#define APIC_SOFTWARE_ENABLE 0x100
#define APIC_REGISTER_EOI 0x0B0

// APIC LVT Timer Flags
#define APIC_LVT_INT_MASKED 0x10000          // Bit 16
#define APIC_LVT_TIMER_MODE_PERIODIC 0x20000 // Bit 17

// IDT vector used for the APIC
#define APIC_VECTOR 200

volatile uint64_t system_ticks = 0;

void apic_timer_handler() {
    system_ticks++;

    // Acknowledge the interrupt so the APIC sends the next one
    apic_write(APIC_REGISTER_EOI, 0);
}

void apic_write(uint32_t reg, uint32_t value) {
    *((volatile uint32_t *)(0xFEE00000 + reg)) = value;
}

uint32_t apic_read(uint32_t reg) {
    return *((volatile uint32_t *)(0xFEE00000 + reg));
}

void apic_start_timer() {
    // Map the spurious interrupt to vector 39 and set the Software Enable bit
    apic_write(APIC_REGISTER_SPURIOUS, 39 | APIC_SOFTWARE_ENABLE);

    // Tell APIC timer to use divider 16
    apic_write(APIC_REGISTER_TIMER_DIV, 0x3);

    // Prepare the PIT to sleep for 10ms (10000µs)
    pit_prepare_sleep(10000);

    // Set APIC init counter to -1
    apic_write(APIC_REGISTER_TIMER_INITCNT, 0xFFFFFFFF);

    // Perform PIT-supported sleep
    pit_perform_sleep();

    // Stop the APIC timer
    apic_write(APIC_REGISTER_LVT_TIMER, APIC_LVT_INT_MASKED);

    // Now we know how often the APIC timer has ticked in 10ms
    uint32_t ticksIn10ms = 0xFFFFFFFF - apic_read(APIC_REGISTER_TIMER_CURRCNT);

    // Start internal APIC timer as periodic on IDT vector 200, divider 16,
    // using calibrated ticks
    apic_write(APIC_REGISTER_LVT_TIMER,
               APIC_VECTOR | APIC_LVT_TIMER_MODE_PERIODIC);
    apic_write(APIC_REGISTER_TIMER_DIV, 0x3);
    apic_write(APIC_REGISTER_TIMER_INITCNT, ticksIn10ms);
}

uint64_t apic_get_ticks(void) { return system_ticks; }