[bits 16]

; elevate 16-bit real mode to 32-bit protected mode

elevate_bios:
    ; clear / disable interrupts
    cli
    ; load gdt
    lgdt [gdt_32_descriptor] 
    ; enable 32-bit mode by setting bit 0 of control register 0. Can't modify cr0 directly so copy into eax and then back again
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    
    ; "far" jump to protected mode
    jmp code_seg:init_protected_mode

    [bits 32]
    init_protected_mode:
    ; in 32-bit PM mode, now need to load DS, ES, FS, GS, SS, ESP
    ; i.e. tell all segment registers to point to flat-mode data segment and then do stack stuff
    mov ax, data_seg
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; make a new stack
    mov ebp, 0x90000
    mov esp, ebp

    ; go to second sector now with 32-bit code
    jmp begin_protected_mode

