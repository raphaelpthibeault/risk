[bits 32]

; clear VGA memory i.e. writes blank spaces to every character slot
clear_protected:
    ; pusha/popa command pushes and pop all registers
    pusha
    ; set up constrains
    mov ebx, vga_size
    mov ecx, vga_start
    mov edx, 0

    clear_protected_loop:
        ; while edx < ebx
        cmp edx, ebx
        jge clear_protected_end

        ; free edx for use later
        push edx 

        ; mov character to al, style to ah
        mov al, space_char
        mov ah, style_wb

        ; print to VGA memory
        add edx, ecx
        mov word[edx], ax
        
        ; restore edx
        pop edx 

        ; increment counter
        add edx, 2 ; char := 2 bytes

        jmp clear_protected_loop
    
clear_protected_end:
    popa
    ret

space_char:                     equ ` `

