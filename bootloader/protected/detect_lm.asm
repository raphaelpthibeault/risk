[bits 32]

; Detecting Long Mode
; - detection of CPUID
; - check for extended functions of CPUID needed to enable long mode
; - check for long mode support
detect_lm_protected:
    ; save state
    pushad ; push all general-purpose registers

    ; check if CPUID is supported by trying to flip the ID bit (bit 21) 
    ; in the FLAGS register. If flippable, CPUID is available
    
    ; copy FLAGS into EAX via stack
    pushfd
    pop eax
    
    ; copy to ECX as well for comparing later on
    mov ecx, eax 

    ; flip ID bit
    xor eax, 1 << 21

    ; copy EAX to flags via the stack
    push eax
    popfd

    ; copy flags back to EAX 
    pushfd
    pop eax

    ; restore flags from ECX (flipping the ID bit back if it was flipped)
    push ecx
    popfd

    ; compare. If equal then bit got flipped back by CPU so CPUID isn't supported
    cmp eax, ecx
    je cpuid_not_supported_protected
    
    ; check CPUID for the function to switch to long mode

    mov eax, 0x80000000     ; set the A-register to 0x80000000
    cpuid                   ; CPU identification
    cmp eax, 0x80000001     ; compare the A-register with 0x80000001
    jb cpuid_not_supported_protected

    ; now to actually check for long mode support using the extended function we know exists

    mov eax, 0x80000001     ; set the A-register to 0x80000001
    cpuid
    test edx, 1 << 29       ; test if the LM-bit (bit 29) is set in the D-register
    jz lm_not_supported_protected

    popad
    ret
    

cpuid_not_supported_protected:
    call clear_protected
    mov esi, cpuid_not_supported_str
    call print_protected
    jmp $

lm_not_supported_protected:
    call clear_protected
    mov esi, lm_not_supported_str
    call print_protected
    jmp $

cpuid_not_supported_str:    db `ERROR: CPUID unsupported, but required for long mode`, 0
lm_not_supported_str:       db `ERROR: Long mode not supported. Exiting...`, 0

