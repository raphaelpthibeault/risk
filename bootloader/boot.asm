[org 0x7C00]

; all x86-type chips start in 16-bit real mode which enables backwards compatibility all the way back to
; Intel 8086
[bits 16]


; initialize base pointer and stack pointer
mov bp, 0x0500
mov sp, bp

; save ID of boot drive which BIOS stores in dl
mov byte[boot_drive], dl

mov bx, msg_hello_world
call print_bios

; load the next sector
; numbering of sectors begins at 1 and the first sector of the 0th cylinder is the boot sector, which has already been loaded
; so we load sector #2
mov bx, 0x002

; load only 1 sector for now
mov cx, 0x001

; want to store the new sector immediately after the first loaded sector
mov dx, 0x7E00

; actually load the sectors
call load_bios

; elevate CPU to 32-bit PM
call elevate_bios

bootsector_hold:
jmp $ ; infinite loop

; INCLUDE real-mode functions
%include "real/print.asm"
%include "real/print_hex.asm"
%include "real/load.asm"
%include "real/gdt.asm"
%include "real/elevate.asm"

; DATA STORAGE AREA
msg_hello_world:                db `\r\nHello World, from the BIOS!\r\n`, 0
boot_drive:                     db 0x00

; pad boot sector for magic number
times 510 - ($ - $$) db 0x00
; magic number
dw 0xAA55


; BEGIN 32-BIT PROTECTED MODE

bootsector_extended:
begin_protected_mode:

; clear VGA memory output
call clear_protected

; test VGA-style print
mov esi, protected_alert
call print_protected

jmp $ ; infinite loop

; INCLUDE protected-mode functions
%include "protected/clear.asm"
%include "protected/print.asm"

; DATA STORAGE AREA
vga_start:                      equ 0x000B8000
vga_size:                       equ 80*25*2     ; VGA memory is 80 chars wide, 25 chars high. 1 char := 2 bytes
style_wb:                       equ 0x0F

protected_alert:                db `Now in 32-bit protected mode`, 0

; pad sector
times 512 - ($ - bootsector_extended) db 0x00

