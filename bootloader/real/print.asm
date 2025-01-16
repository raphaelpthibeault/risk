[bits 16]

print_bios:
    ; save ax and bx onto the stack
    push ax
    push bx
    
    ; interrupt 0x10 does multiple things. Of primary interest is print to screen
    ; which needs 0x0E into HIGH 8 bits of ax, which can be referred to as ah
    ; (the low bits would be al)

    mov ah, 0x0E
    ; loop over the string
    print_bios_loop:
        ; comparison to null operator. If string isn't null terminated then uh-oh
        cmp byte[bx], 0
        je print_bios_end
        
        mov al, byte[bx]
        int 0x10 

        inc bx 
        jmp print_bios_loop

print_bios_end:
    pop bx 
    pop ax 

    ret

