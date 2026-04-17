#include "src/gdt.h"
#include "src/idt.h"
#include "src/drivers/apic.h"
#include "src/io.h"
#include "src/drivers/vga.h"
#include "src/kernel/timer.h"
#include "multiboot.h"
#include "src/kernel/panic.h"
#include <stdint.h>


int kmain(multiboot_info_t* mbd, uint32_t magic) {
    clear_screen();

    /* Make sure the magic number matches for memory mapping*/
    if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("invalid magic number!");
    }

    /* Check bit 6 to see if we have a valid memory map */
    if(!(mbd->flags >> 6 & 0x1)) {
        panic("invalid memory map given by GRUB bootloader");
    }

    /* Loop through the memory map and display the values */
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*) mbd->mmap_addr;
    
    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length) {
        
        kprintf("Start Addr: %x | Length: %x | Type: %d\n",
            (uint32_t)mmap->addr, (uint32_t)mmap->len, mmap->type);

        if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // This is usable RAM!
        }

        // Safely advance to the next entry using the entry's actual size
        mmap = (multiboot_memory_map_t*) ( (uint32_t)mmap + mmap->size + sizeof(mmap->size) );
    }



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