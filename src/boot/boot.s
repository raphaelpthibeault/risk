; x86_64 entry point for the bootloader
; https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html
%define KERNEL_VIRTUAL_ADDRESS 0xFFFFFFFF80000000
%define PAGE_DIR_ENTRY_FLAGS 0b11
%define PRESENT_BIT 1
%define WRITE_BIT 0b10
%define HUGEPAGE_BIT 0b10000000

%define PAGE_SIZE 0x200000
%define PAGE_TABLE_ENTRY HUGEPAGE_BIT | PRESENT_BIT | WRITE_BIT ; PAGE_TABLE (pd table) entry for 2M pages, huge page bit is set.
%define LOOP_LIMIT 512

%define MULTIBOOT_TAG_TYPE_END  0
%define MULTIBOOT_TAG_TYPE_BASIC_MEMINFO 4
%define MULTIBOOT_TAG_TYPE_MMAP 6
%define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8
%define MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR 21
%define MULTIBOOT_TAG_TYPE_ACPI_OLD 14
%define MULTIBOOT_TAG_TYPE_ACPI_NEW 15

struc multiboot_tag
    .type:  resd 1
    .size:  resd 1
endstruc

struc multiboot_tag_framebuffer
    .type:  resd 1
    .size:  resd 1
    .framebuffer_addr:  resq 1
    .framebuffer_pitch: resd 1
    .framebuffer_width: resd 1
    .framebuffer_height:    resd 1
    .framebuffer_bpp:   resb    1
    .framebuffer_type:  resb    1
    .reserved:  resb 1
endstruc

section .multiboot_header
%define magic_number 0xe85250d6
%define arch 0 ; protected mode
%define length header_end - header_start
%define chksum 0x100000000 - (magic_number + arch + length)

header_start:
    align 8
    dd magic_number
    dd arch
    dd length
    dd chksum

framebuffer_tag_start:
    dw 0x05 ; type framebuffer 
    dw 0x01 ; option flag
    dd framebuffer_tag_end - framebuffer_tag_start ; length
    dd 0 ; width, let bootloader decide
    dd 0 ; height, let bootloader decide
    dd 0 ; depth, let bootloader decide
framebuffer_tag_end:
    
    align 8
    dw 0 ; type
    dw 0 ; flags
    dd 8 ; length

header_end:

section .multiboot.text
global start
global pml2_table
global pml3_table
global pml3_table_hh
global pml4_table
global end_of_mapped_memory ; will contain the virtual address of the last address mapped after bootstrap
global multiboot_framebuffer_data
global multiboot_mmap_data
global multiboot_basic_meminfo
global multiboot_acpi_info
global read_multiboot
global gdt
global stack

extern kernel_start

[bits 32]

start:
    mov edi, ebx ; mulitboot structure
    mov esi, eax ; magic number

    mov esp, stack.top - KERNEL_VIRTUAL_ADDRESS

    ; temporary paging
    mov eax, pml3_table - KERNEL_VIRTUAL_ADDRESS ; pml3 table address
    or eax, PRESENT_BIT | WRITE_BIT ; set present and writable bits to 1
    mov dword [(pml4_table - KERNEL_VIRTUAL_ADDRESS) + 0], eax ; copy pml3 table address content into entry 0 of pml4 table
    
    mov eax, pml3_table_hh - KERNEL_VIRTUAL_ADDRESS ; pml3 table address in the higher half
    or eax, PRESENT_BIT | WRITE_BIT 
    mov dword [(pml4_table - KERNEL_VIRTUAL_ADDRESS) + 511 * 0x08], eax ; copy pml3 table address in higher half to last entry of pml4 (512 entries) , pml entry size := 0x08

    mov eax, pml4_table - KERNEL_VIRTUAL_ADDRESS ; map pml4 table address into itself at 2nd to last entry
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(pml4_table - KERNEL_VIRTUAL_ADDRESS) + 510 * 0x08], eax

    ; now for pml2
    mov eax, pml2_table - KERNEL_VIRTUAL_ADDRESS
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(pml3_table - KERNEL_VIRTUAL_ADDRESS) + 0], eax

    mov eax, pml2_table - KERNEL_VIRTUAL_ADDRESS
    or eax, PRESENT_BIT | WRITE_BIT
    mov dword [(pml3_table_hh - KERNEL_VIRTUAL_ADDRESS) + 510 * 0x08], eax

    ; loop 
    mov ecx, 0 ; counter
    .map_pml2_table:
        mov eax, PAGE_SIZE ; move page size into the accumulator
        mul ecx ; multiply by counter to get current entry
        or eax, PAGE_TABLE_ENTRY ; set: present, writable, and huge page bit, in eax

        mov [(pml2_table - KERNEL_VIRTUAL_ADDRESS) + ecx * 0x08], eax ; place the set bits into current entry
        
        inc ecx
        cmp ecx, LOOP_LIMIT
        jne .map_pml2_table
    
    ; almost ready to jump to long mode

    ; set cr3; can't modify control registers directly though,
    ; always must use another register
    mov eax, (pml4_table - KERNEL_VIRTUAL_ADDRESS)
    mov cr3, eax
    
    ; set PAE 
    mov eax, cr4
    or eax, 1 << 5 ; physical address extension bit
    mov cr4, eax

    ; setup long mode bit
    mov ecx, 0xC0000080
    ; rdmsr: read msr (model specific register) into eax
    rdmsr
    or eax, 1 << 8
    ; wrmsr: write eax into msr
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31 ; paging bit
    or eax, 1 << 16 ; write protection. CPU now shouldn't be able to write to read-only pages when privilege level is 0
    mov cr0, eax

    ; load gdt
    lgdt [gdt.pointer_low - KERNEL_VIRTUAL_ADDRESS]
    jmp (0x8):(kernel_jumper - KERNEL_VIRTUAL_ADDRESS)

    bits 64


section .text 
; jump to long mode
kernel_jumper:
    bits 64
    mov qword [(end_of_mapped_memory - KERNEL_VIRTUAL_ADDRESS)], (511 << 39) | (510 << 30) | (511 << 21)
    ; reload segment selectors
    mov ax, 0x10
    mov ss, ax  ; stack segment selector
    mov ds, ax  ; data segment register
    mov es, ax  ; extra segment register
    mov fs, ax  ; extra segment register
    mov gs, ax  ; extra segment register

    lea rax, [rdi + 8]
    
    ; in theory the bss section should be 0 (unix and windows) so initializing would be pointless

read_multiboot:
    ; see if tag is required by kernel, if yes store its address
    cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_FRAMEBUFFER
    je .parse_framebuffer_data
    cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_MMAP
    je .mmap_tag_item
    cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_BASIC_MEMINFO
    je .meminfo_tag_item
    cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_ACPI_OLD
    je .acpi_item
    cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_ACPI_NEW
    je .acpi_item
    jmp .item_not_needed

    .parse_framebuffer_data:
        mov [multiboot_framebuffer_data], rax
        ; map first 4MB of framebuffer
        ; the rest of the initialization will be done by the kernel
        mov rbx, [(rax + multiboot_tag_framebuffer.framebuffer_addr)]
        or rbx, PAGE_TABLE_ENTRY
        mov qword [(pml2_table - KERNEL_VIRTUAL_ADDRESS) + 8 * 488], rbx
        add rbx, PAGE_SIZE
        or rbx, PAGE_TABLE_ENTRY
        mov qword [(pml2_table - KERNEL_VIRTUAL_ADDRESS) + 8 * 489], rbx
        jmp .item_not_needed
    .mmap_tag_item:
        mov [multiboot_mmap_data], rax
        jmp .item_not_needed
    .acpi_item:
        mov [multiboot_acpi_info], rax
        jmp .item_not_needed
    .meminfo_tag_item:
        mov [multiboot_basic_meminfo], rax
    .item_not_needed:
        mov ebx, [rax + multiboot_tag.size]
        ; next tag will be at current_tag_addr + current_tag_size
        add rax, rbx
        ; padded with 0 until the first byte that's 8-byte aligned
        add rax, 7
        and rax, ~7
        ; check if tag is the end tag (which has type 0 and size 8), if no re-read
        cmp dword [rax + multiboot_tag.type], MULTIBOOT_TAG_TYPE_END
        jne read_multiboot
        cmp dword [rax + multiboot_tag.size], 8
        jne read_multiboot
    
    mov rax, higher_half
    jmp rax

higher_half:
    ; far jump to long mode

    mov rsp, stack.top 
    lgdt [gdt.pointer]

    ; unmap lower half 
    mov eax, 0x0
    mov dword [(pml4_table - KERNEL_VIRTUAL_ADDRESS) + 0], eax

    mov rax, cr3
    mov cr3, rax
    call kernel_start

section .bss 

align 4096
pml4_table: ; PML4
    resb 4096
pml3_table: ; PDPR
    resb 4096
pml3_table_hh: ; PDPR
    resb 4096
pml2_table: ; PDP
    resb 4096

; multiboot info
align 4096
end_of_mapped_memory:
    resq 1
multiboot_framebuffer_data:
    resb 8
multiboot_mmap_data:
    resb 8
multiboot_basic_meminfo:
    resb 8
multiboot_acpi_info:
    resb 8
stack:
    resb 16384
    .top:

section .data
; gdts can contain up to 0x2000 (8192) entries, but I define 7
; entry 0: null, is always null
; entry 1-2: data and code segments for kernel
; entry 3-4: data nad code segments for user
; entry 5-6: TSS lower and upper parts
; now a gdt entry is of the following form:
gdt:
    dq 0 ; first entry, null
    ; equ specifies to the compiler to set the value of the variable to given value. $ means current position
    .code equ $ - gdt ; second entry ::= kernel code @ 0x8
        dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53)  
    .data equ $ - gdt ; third entry ::= kernel data @ 0x10
        dq (1 << 44) | (1 << 47) | (1 << 41)	
    .ucode equ $ - gdt ; fourth entry ::= user code @ 0x18
        dq (1 << 44) | (1 << 47) | (1 << 41) | (1 << 43) | (1 << 53) | (3 << 45) 
    .udata equ $ - gdt ; fifth entry ::= user data @ 0x20
        dq (1 << 44) | (1 << 47) | (1 << 41) | (3 << 45)	
    .tss_low equ $ - gdt ; sixth and seventh entries and TSS part placeholders
        dq 0
    .tss_high equ $ - gdt
        dq 0

.pointer: 
    dw .pointer - gdt - 1
    dq gdt

.pointer_low:
    dw .pointer - gdt - 1
    dq gdt - KERNEL_VIRTUAL_ADDRESS
