[bits 16]

; printing a hex. Assume conversion is already done so we're just trying to print a 16-bit number
; input in bx
print_hex_bios:
    ; save state
    push ax
    push bx
    push cx

    ; enable print mode
    mov ah, 0x0E

    ; print prefix
    mov al, '0'
    int 0x10 
    mov al, 'x'
    int 0x10
    
    ; init cx as counter of nibbles left (4 nibbles in 16 bits)
    mov cx, 4

    print_hex_bios_loop:
        cmp cx, 0
        je print_hex_bios_end

        ; save the to-print input
        push bx

        ; idea for here is to put the character we want to print in al
        ; if numeric then 0 + bl (bx low [as opposed to bh which is bx high]) in al
        ; if alpha then 'A' + bl after subtracting 10 to get char num

        ; right shift so that upper 4 bits become lower 4 bits
        shr bx, 12
        
        ; if >= 10, then alphabet
        cmp bx, 10
        jge print_hex_bios_alpha

            ; if not, numeric
            ; set zero char in al, add bl
            mov al, '0'
            add al, bl

            ; jump to end of loop
            jmp print_hex_bios_got_char

        print_hex_bios_alpha:
            ; subtract 10 to get add amount
            sub bl, 10
            ; move 'A'to al and add bl
            mov al, 'A'
            add al, bl

        print_hex_bios_got_char:

        ; now we have char to print in al
        ; so print it
        int 0x10
        
        ; restore bx
        pop bx
        ; left shift to next 4 bits
        shl bx, 4

        ; decrement nibble count
        dec cx

        jmp print_hex_bios_loop
    
print_hex_bios_end:
    pop cx
    pop bx
    pop ax

    ret
