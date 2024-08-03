SECTION .text

GLOBAL	idt_load
; convention: rdi holds idt_descriptor and rsi holds enable_interrupts
idt_load:
	cli
	lidt	[rdi]
	cmp		rsi, 0
	jz		.exit
	sti	
.exit:
	ret
