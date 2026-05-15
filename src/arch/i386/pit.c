#include <arch/i386/io.h>

#define PIT_CMD_PORT 0x43
#define PIT_CH2_PORT 0x42
#define PIT_GATE_PORT 0x61

#define PIT_FREQ 1193182

void pit_prepare_sleep(uint32_t microseconds) {
    uint32_t reload_value =
        (uint32_t)(((uint64_t)PIT_FREQ * microseconds) / 1000000);

    uint8_t gate = inb(PIT_GATE_PORT);
    outb(PIT_GATE_PORT, gate | 0x01);

    // 0xB0 = 10110000 in binary:
    // Bits 6-7: 10 (Channel 2)
    // Bits 4-5: 11 (Access LSB, then MSB)
    // Bits 1-3: 000 (Mode 0: Interrupt on terminal count)
    // Bit  0  : 0 (16-bit binary mode)
    outb(PIT_CMD_PORT, 0xB0);

    outb(PIT_CH2_PORT, (uint8_t)(reload_value & 0xFF));      // LSB
    outb(PIT_CH2_PORT, (uint8_t)(reload_value >> 8) & 0xFF); // MSB
}

void pit_perform_sleep() {
    while ((inb(PIT_GATE_PORT) & 0x20) == 0) {
        __asm__ volatile("pause");
    }
}