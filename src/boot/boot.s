; x86_64 entry point for the bootloader
; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

section .multiboot
[bits 32]

extern phys
extern bss_start
extern end

; Multiboot 2 Header
%define mb2_magic_number 0xe85250d6
%define mb2_arch 0 ; protected mode
%define mb2_length multiboot2_header_end - multiboot2_header
%define mb2_chksum -(mb2_magic_number + mb2_arch + mb2_length)
align 8
multiboot2_header:
    dd mb2_magic_number
    dd mb2_arch
    dd mb2_length
    dd mb2_chksum

; Address tag
align 8
mb2_tag_addr:
    dw 2
    dw 0
    dd 24
    dd multiboot2_header
    dd phys
    dd bss_start
    dd end

; Entry tag
align 8
mb2_tag_entry:
    dw 3
    dw 0
    dd 12
    dd start_mb2

; Framebuffer tag
align 8
mb2_tag_fb:
    dw 5
    dw 0
    dd mb2_tag_fb_end - mb2_tag_fb
    dd 0 ; 1920 x 1080 in the future, for now let bootloader decide
    dd 0
    dd 0
mb2_tag_fb_end:

    align 8
    dw 4
    dw 1
    dd 12
    dd 2 ; supports EGA text 

    align 8 ; align modules
    dw 6
    dw 0
    dd 8
    
    ; relocatable tag 
    align 8
    dw 10
    dw 0
    dw 24
    dd 0x100000 ; start
    dd 0x1000000 ; maximum load address
    dd 0x1000 ; request page alignment
    dd 1 ; load at lowest available

    ; end tag
    align 8
    dw 0
    dw 0
    dd 8

multiboot2_header_end:

section .bootstrap
[bits 32]
align 4

extern kernel_start
global start_mb2
start_mb2:
    ; use reserved 0 space of boot information struct to thunk eip
    mov ecx, ebx
    add ecx, 8
    jmp go_forward
    

global start
start:
    ; thunk eip
    mov ecx, ebx
    add ecx, 16
go_forward:
    mov esp, ecx
    call _forward

_forward:
    pop ecx
    sub ecx, _forward
    ; setup stack
    mov esp, stack_top
    add esp, ecx

    ; assert stack is 16-byte aligned
    and esp, -16

    push dword 0
    push esp
    push dword 0
    push eax ; mb2 header magic
    push dword 0
    push ebx ; mb2 header pointer

    jmp jump_to_long


align 4
jump_to_long:
    extern page_region_init

zero_loop_begin:
    push ecx

    ; zero out page_region_init hardcoded 3 * 512, each 8 bytes
    ; have to zero it out since it's in the data section, so it'll be random junk data since it's 
    ; only declared, not initialized in the C code
    mov edi, page_region_init
    mov ecx, (3 * 512)          ; number of 8-byte qwords 
    xor eax, eax                ; set eax to 0 (the value to store)
zero_loop:
    stosd
    stosd 
    loop zero_loop ; decrement ecx and repeat until ecx == 0

    pop ecx
zero_loop_end:

    ; setup paging region, which should have been zeroed out
    mov edi, page_region_init
    add edi, ecx
    push ecx

    ; PML4[0] = &PDP[0] | (PRESENT, WRITABLE, USER)
    mov eax, 0x1007
    add eax, edi
    mov [edi], eax ; 10b007

    ; PDP[0] = &PD[0] | (PRESENT, WRITABLE, USER)
    add edi, 0x1000 ; next 4 kib page
    mov eax, 0x1003
    add eax, edi
    mov [edi], eax

    ; setup temp memory. 32 MiB pages mapping 64 MiB should be enough to get to
    ; the memory management unit where we'll do 4-level paging
    ; that maps all of the 4 GiB standard memory space
    add edi, 0x1000 ; next 4 kib page

    mov ebx, 0x87
    mov ecx, 32

set_entry:
    mov [edi], ebx
    add ebx, 0x200000
    add edi, 8
    loop set_entry

    mov edi, page_region_init
    pop ecx
    add edi, ecx
    push ecx
    mov cr3, edi ; set page directory base register

    ; enable PAE (physical address extension)
    mov eax, cr4 
    or eax, 32  ; sets bit 5
    mov cr4, eax

    ; update EFER (extended feature enable register)
    mov ecx, 0xC0000080 ; loads EFER MSR (model specific register)
    rdmsr ; read msr from ecx, result is in eax and edx
    or eax, 256 ; set bit 8 (LME) to enable long mode
    wrmsr ; write modified value back into msr

    ; check if paging is already enabled e.g. booting with EFI
    mov eax, cr0
    test eax, 0x80000000 ; test if paging bit (bit 31) is set
    jnz continue ; if set, continue

    ; else, enable paging
    or eax, 0x80000000
    mov cr0, eax
    
;super_duper_bullshit:
    pop ecx
    
    lea eax, [_lgdt_instr + 3 + ecx]
    mov ebx, [eax]
    add ebx, ecx
    mov [eax], ebx

    lea eax, [gdtr + 2 + ecx]
    mov ebx, [eax]
    add ebx, ecx
    mov [eax], ebx

    lea eax, [_ljmp_instr + 1 + ecx]
    mov ebx, [eax]
    add ebx, ecx 
    mov [eax], ebx 

    push dword 0
    push ecx

_lgdt_instr:
    lgdt [gdtr]

_ljmp_instr:
    jmp 0x08:realmode64
    ;ljmp 0x08, realmode64 ; perform long jump to segment 0x08 (kernel code) at label realmode64

align 8
gdtr:
    dw gdt_end - gdt_base   ; limit
    dq gdt_base             ; base

; temporary gdt structure that will only hold kernel code and data
gdt_base:
    ; null descriptor
    dq 0
    ; kernel code
    dw 0                            ; limit 0-15
    dw 0                            ; base 0-15
    db 0                            ; base 16-23
    db 0x9a                         ; access: present, ring 0, executable
    db 0x20                         ; flags and limit 16-19
    db 0                            ; base 24-31
    ; kernel data
    dw 0xffff                       ; limit 0-15
    dw 0                            ; base 0-15 bits
    db 0                            ; base 16-23 bits
    db 0x92                         ; access byte: present, ring 0, readable/writable
    db 0                            ; flags and limit 16-19
    db 0                            ; base 24-31 
gdt_end:


[bits 64]
align 8
section .bootstrap

realmode64:
    cli 
    mov ax, 0x10
    mov ds, ax 
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

continue:
    cli 
    pop rcx
    pop rdi
    pop rsi
    pop rdx
    call kernel_start

halt:
    cli 
    hlt
    jmp halt

; stack is in bss
section .bss
align 16
;section .stack
stack_bottom:
    resb 16384 ; 16 KiB
global stack_top
stack_top:

