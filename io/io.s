global outb             ; make the label outb visible outside this file

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8]    ; Move the data byte from the stack to AL register
    mov dx, [esp + 4]    ; Move the address of the I/O port from the stack to DX register
    out dx, al           ; Send the data byte to the I/O port specified by DX
    ret                  ; Return from the function
