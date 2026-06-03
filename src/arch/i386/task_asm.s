global switch_to_task

; void switch_to_task(void **old_esp_ptr, void *new_esp);
switch_to_task:
    ; save current thread state
    ; cdecl only requires us to store ebp, ebx, esi och edi
    pushfd
    push ebp
    push ebx
    push esi
    push edi

    ; save old esp
    mov eax, [esp + 24]     ; old_esp_ptr
    mov [eax], esp

    ; switch to new stack
    mov esp, [esp + 28]     ; new_esp

    ; restore new thread state
    ; as required by cdecl, i.e. ebp, ebx, esi och edi 
    pop edi
    pop esi
    pop ebx
    pop ebp
    popfd

    ret