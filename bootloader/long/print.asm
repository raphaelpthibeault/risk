[bits 64]
; input: style in rdi, string in rsi
print_long:
    push rax
    push rdx
    push rdi 
    push rsi 

    mov rdx, vga_start
    shl rdi, 8

    print_long_loop:
        cmp byte[rsi], 0
        je print_long_end
        ; check length
        cmp rdx, vga_start + vga_size
        je print_long_end

        ; move char to al, style to ah
        mov rax, rdi
        mov al, byte[rsi]

        ; print to vga memory location
        mov word[rdx], ax

        add rsi, 1
        add rdx, 2

        jmp print_long_loop

print_long_end:
    pop rsi 
    pop rdi 
    pop rdx 
    pop rax

    ret

