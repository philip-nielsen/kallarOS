global isr_apic_timer
global isr_spurious

extern apic_timer_handler 

section .text

isr_apic_timer:
    pushad                  ; Save all general-purpose registers (EAX, EBX, ECX, etc.)

    cld                     ; Clear the direction flag
    
    call apic_timer_handler 

    popad                   ; Restore all general-purpose registers
    iretd                   ; Return

isr_spurious:
    iretd                   ; Just immediately return