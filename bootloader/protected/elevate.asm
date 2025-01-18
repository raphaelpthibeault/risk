
; elevate 32-bit protected mode to 64-bit long mode
; - enable long mode 
; - enable paging
; - load 64-bit gdt
; - far jump
[bits 32]
elevate_protected:
    ; set the long mode bit
    mov ecx, 0xC0000080          ; set the C-register to 0xC0000080, which is the EFER MSR (Extended Feature Enable Register)(model-specific register).
    rdmsr                        ; read from the model-specific register.
    or eax, 1 << 8               ; set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        ; write to the model-specific register.

    ; enable paging
    mov eax, cr0                 ; set the A-register to control register 0.
    or eax, 1 << 31              ; set the PG-bit, which is the 32nd bit (bit 31).
    mov cr0, eax                 ; set control register 0 to the A-register.

    lgdt[gdt_64_descriptor]
    jmp code_seg_64:init_lm

    [bits 64]
    init_lm:
    cli 
    mov ax, data_seg_64
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp begin_long_mode
