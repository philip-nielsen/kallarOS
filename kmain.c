#include "src/gdt.h"
#include "src/idt.h"
#include "src/drivers/apic.h"
#include "src/io.h"
#include "src/drivers/vga.h"
#include "src/timer.h"


int kmain() {
    clear_screen();
    print("Booting OS\n");
    initGdt();
    print("GDT initialized\n");
    idt_init();
    print("IDT initialized\n");
    outb(0x21, inb(0x21) | 0x01); // MASK LEGACY IRQ 0 (PIT)

    apic_start_timer();
    print("APIC timer started\n");
    
    uint32_t uptime_seconds = 0;
    char spinner[] = {'|', '/', '-', '\\'};

    print("System Uptime: \n");

    while (1) {
        // \r brings us back to the start of the line so we overwrite the old time
        print("\r  [");
        
        // Pick a spinner frame based on the time
        char current_spin[2] = { spinner[uptime_seconds % 4], '\0' };
        print(current_spin);
        
        print("] ");
        print_int(uptime_seconds);
        print(" seconds active...");
        
        sleep_ms(1000); 
        uptime_seconds++;
    }
}