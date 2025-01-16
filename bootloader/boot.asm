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

; should be able to read the loaded string
mov bx, loaded_msg
call print_bios


bootsector_hold:
jmp $ ; infinite loop

; UTIL FUNCTION AREA
%include "print.asm"
%include "print_hex.asm"
%include "load.asm"

; DATA STORAGE AREA
msg_hello_world:                db `\r\nHello World, from the BIOS!\r\n`, 0
boot_drive:                     db 0x00

; pad boot sector for magic number
times 510 - ($ - $$) db 0x00
; magic number
dw 0xAA55

bootsector_extended:

loaded_msg:                     db `\r\nNow reading from the next sector!`, 0
; pad sector
times 512 - ($ - bootsector_extended) db 0x00

bu:

