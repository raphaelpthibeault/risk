; Flat / Long Mode GDT Setup
; don't care for segmentation in 64-bit, but must include it for backwards compatibility
; typically this will consist of the null segment, and one code and one data segment for kernel and user mode, and a Task State Segment
; but I'll just do null, code and data for now

[bits 16]

gdt_32_start:

gdt_32_null:
    dd 0x00000000
    dd 0x00000000

gdt_32_code:
    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10011010       ; 1st Flags, Type flags
    db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

gdt_32_data:
    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0x0000           ; Base  (bits 0-15)
    db 0x00             ; Base  (bits 16-23)
    db 0b10010010       ; 1st Flags, Type flags
    db 0b11001111       ; 2nd Flags, Limit (bits 16-19)
    db 0x00             ; Base  (bits 24-31)

gdt_32_end:

; descriptor for the gdt which gives the cpu the gdt's start address and length
gdt_32_descriptor:
    dw gdt_32_end - gdt_32_start - 1    ; length, 1 byte less than true size
    dd gdt_32_start                     ; start address

; helpers for pointer finding in the segments
code_seg: equ gdt_32_code - gdt_32_start
data_seg: equ gdt_32_data - gdt_32_start

