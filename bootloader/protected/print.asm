[bits 32]

; we no longer have access to BIOS utilities, so use a simple VGA print for now
; input:
;   esi: message address
print_protected:
    pusha
    mov edx, vga_start

    print_protected_loop:
        ; if null-terminated, done
        cmp byte[esi], 0
        je print_protected_end

        ; character to al, style to ah
        mov al, byte[esi]
        mov ah, style_wb

        ; print char
        mov word[edx], ax

        ; incr counter registers
        add esi, 1 
        add edx, 2

        jmp print_protected_loop

print_protected_end:
    popa
    ret

