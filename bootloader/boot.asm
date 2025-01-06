[org 0x7C00]

; all x86-type chips start in 16-bit real mode which enables backwards compatibility all the way back to
; Intel 8086
[bits 16]

mov bx, msg_hello_world
call print_bios

bootsector_hold:
jmp $ ; infinite loop

; UTIL FUNCTION AREA
%include "print.asm"

; DATA STORAGE AREA
msg_hello_world:                db `\r\nHello World, from the BIOS!\r\n`, 0

; pad boot sector for magic number
times 510 - ($ - $$) db 0x00
; magic number
dw 0xAA55

