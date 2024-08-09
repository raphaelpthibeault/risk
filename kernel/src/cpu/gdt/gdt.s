SECTION .text

global gdt_reload
gdt_reload:
    cli
    lgdt [rdi]
    pop rsi                 ; keep the return address in RSI
    mov		rdi, rsp		; keep the old stack pointer in rdi
	mov		rax, 0x10		; keep kernel data segments
	mov		ds, ax
	mov		es, ax
	; push what iretq needs
	push	rax				; push stack segment
	push	rdi				; push old stack pointer
	pushfq					; push flags
	push	QWORD 0x08	    ; push kernel code segment
	push	rsi				; set rel to the return address from earlier
	iretq					; far return in new code segment, and re-enable interrupts
    
global tss_reload
tss_reload:
    mov ax, 0x28
    ltr ax
    ret

