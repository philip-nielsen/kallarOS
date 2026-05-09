global loader                   ; the entry symbol for ELF
extern kmain

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
FLAGS        equ 0x0            ; multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                ; (magic number + checksum + flags should equal 0)

KERNEL_VIRTUAL_BASE equ 0xC0000000 ; Kernel at 3GB

; Macro to convert virtual -> physical
%define PHYS(x) ((x) - KERNEL_VIRTUAL_BASE) 

section .bss
align 4096
; One temporary directory and at least one temporary table needed to cover the first 4MB
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096

align 4
KERNEL_STACK_SIZE equ 4096          ; size of stack in bytes
kernel_stack:                       
    resb KERNEL_STACK_SIZE      ; reserve stack for the kernel

section .text                   ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader:                         ; the loader label (defined as entry point in linker script)
    mov ebp, eax ; save magic number
    ; Build the page table
    mov edi, PHYS(boot_page_table1)
    mov esi, 0           ; Start at physical address 0
    mov ecx, 1024        ; 1024 entries in a table

.fill_table:
    mov edx, esi
    or edx, 3                    ; Set flags Present and R/W
    mov [edi], edx
    add esi, 4096
    add edi, 4
    loop .fill_table

    mov eax, PHYS(boot_page_table1) ; Put table in the directory
    or eax, 3                       ; Present + R/W

    mov [PHYS(boot_page_directory) + 0], eax

    ; 768 is the directory index for 0xC0000000. (0xC0000000 >> 22 = 768)
    ; Each entry is 4 bytes, so 768 * 4 = 3072.
    mov [PHYS(boot_page_directory) + (768 * 4)], eax
    

    ;Map the Recursive Directory (Index 1023)
    mov eax, PHYS(boot_page_directory)
    or eax, 3
    mov [PHYS(boot_page_directory) + (1023 * 4)], eax

    mov cr3, eax                    ; Enable paging

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lea eax, [higher_half_entry]
    jmp eax


higher_half_entry:
    CLI
    mov esp, kernel_stack + KERNEL_STACK_SIZE ; point esp to the start of the stack

    mov dword [boot_page_directory + 0], 0    ; Remove the identity map at index 0.

    ; Reload CR3 to flush the TLB and apply the unmap
    mov eax, cr3
    mov cr3, eax
    ; Pass Multiboot info to kmain (kernel)
    push ebp    ; Push the magic number
    push ebx    ; Push the pointer to the Multiboot Information Structure
    CALL kmain
    HLT

.loop:
    jmp .loop                   ; loop forever