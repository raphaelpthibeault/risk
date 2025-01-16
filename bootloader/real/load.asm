[bits 16]

;  input:
;   bx: sector start point
;   cx: number of sectors to read
;   dx: destination address
load_bios:
    ; save state
    push ax
    push bx 
    push cx
    push dx

    ; save number of sectors to read again, need this later
    push cx

    ; ATA Read BIOS utility, value of ah must be 0x02 
    mov ah, 0x02 

    ; number of sectors to read should be in al, but this function takes it in cl
    mov al, cl

    ; sector to read must be in cl, but this function takes it in bl
    mov cl, bl

    ; destination address must be in bx, but our function takes it in dx
    mov bx, dx 

    ; cylinder and cylinder head to read from
    ; TODO find the values for real drive, but with QEMU using 0x00 is fine
    mov ch, 0x00 ; cylinder
    mov dh, 0x00 ; cylinder head

    ; load the drive
    mov dl, byte[boot_drive]

    int 0x13 ; interrupt for BIOS read

    ; error handlng: if error, BIOS read will set the carry bit in the 8086 special register
    ; we can use 'jc' to jump if carry bit is set
    jc bios_disk_error
    
    ; it can happen that BIOS will not read the requested amount but will return without error
    ; so we check the read amount (in al) with the cx we pushed onto the stack at the start
    pop bx ; pop old cx into bx
    cmp al, bl
    jne bios_disk_error

    mov bx, success_msg
    call print_bios

    ; restore state
    pop dx 
    pop cx 
    pop bx
    pop ax

    ret

bios_disk_error:
    ; print error and hang
    mov bx, error_msg
    call print_bios

    ; error code should be in ah. Right shift to mask al
    shr ax, 8
    mov bx, ax
    call print_hex_bios
    
    ; infinite loop
    jmp $

error_msg:              db `\r\nERROR Loading Sectors. Code: `, 0
success_msg:            db `\r\nAdditional Sectors Loaded Successfully!\r\n`, 0

