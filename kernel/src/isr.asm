%include "kernel.inc"

EXTERN	g_isr_handlers
; EXTERN	local_apic_eoi  have some function to send End of Interrupt signal of the calling APIC

%macro	ISR_DEFINE	2
	%assign	ISR_NUM	%1
	GLOBAL	isr%+ISR_NUM
	isr%+ISR_NUM:
		cli
		%if	%2 == 0
			push	QWORD 0				; push quad so the stack size is consistent
		%endif
		PUSH_REG
		mov		rax, [g_isr_handlers + 8 * ISR_NUM]
		cmp		rax, 0					; avoid null handlers
		jz		%%.finish
		mov		rdi, rsp				; we'll create a struct out of the stack pointer
		mov		rsi, ISR_NUM
		call	rax
		%%.finish:
			POP_REG
			add		rsp, 8
			sti
			iretq
%endmacro

%macro	ISR_DEFINE_IRQ	1
	%assign	ISR_NUM	%1
	GLOBAL	isr%+ISR_NUM
	isr%+ISR_NUM:
		PUSH_REG
		mov		rax, [g_isr_handlers + 8 * ISR_NUM]
		cmp		rax, 0
		jz		%%.finish
		mov		rdi, rsp
		mov		rsi, ISR_NUM
		call	rax
		;call	local_apic_eoi
		%%.finish:
			POP_REG
			iretq
%endmacro

%macro	ISR_DEFINE_DIRECT	1
	%assign	ISR_NUM	%1
	GLOBAL	isr%+ISR_NUM
	isr%+ISR_NUM:
		jmp	[g_isr_handlers + 8 * ISR_NUM]
%endmacro

SECTION .text

; Exceptions
;ISR_DEFINE	0, 0	; Divide By Zero Exception
;ISR_DEFINE	1, 0	; Debug Exception
;ISR_DEFINE	2, 0	; Non Maskable Interrupt Exception
;ISR_DEFINE	3, 0	; Breakpoint Exception
;ISR_DEFINE	4, 0	; Into Detected Overflow Exception
;ISR_DEFINE	5, 0	; Out of Bounds Exception
;ISR_DEFINE	6, 0	; Invalid Opcode Exception
;ISR_DEFINE	7, 0	; No Coprocessor Exception
ISR_DEFINE	8, 1	; Double Fault Exception
;ISR_DEFINE	9, 0	; Coprocessor Segment Overrun Exception
ISR_DEFINE	10, 1	; Invalid Task State Segment
ISR_DEFINE	11, 1	; Segment Not Present Exception
;ISR_DEFINE	12, 1	; Stack Fault Exception
ISR_DEFINE	13, 1	; General Protection Fault Exception
ISR_DEFINE	14, 1	; Page Fault Exception
;ISR_DEFINE	16, 0	; Coprocessor Fault Exception
;ISR_DEFINE	17, 1	; Alignment Check Exception (486+)
;ISR_DEFINE	18, 0	; Machine Check Exception (Pentium/586+)
;ISR_DEFINE	19, 0	; SIMD Floating-Point Exception
;ISR_DEFINE	20, 0	; Virtualization Exception
;ISR_DEFINE	30, 1	; Security Exception
; Interrupt requests
; ISR_DEFINE_IRQ	32	; PIT
ISR_DEFINE_IRQ	33	; Keyboard Interrupt
;ISR_DEFINE_IRQ	34	; Cascade
ISR_DEFINE_IRQ	35	; COM2
ISR_DEFINE_IRQ	36	; COM1
ISR_DEFINE_IRQ	37	; LPT2
ISR_DEFINE_IRQ	38	; Floppy disk
; ISR_DEFINE_IRQ	39	; LPT (spurious interrupt) (parallel port)
ISR_DEFINE_IRQ	40	; CMOS
ISR_DEFINE_IRQ	41	; Peripherals 1 (legacy SCSI/NIC)
ISR_DEFINE_IRQ	42	; Peripherals 2 (SCSI/NIC)
ISR_DEFINE_IRQ	43	; Peripherals 3 (SCSI/NIC)
ISR_DEFINE_IRQ	44	; PS2 Mouse
ISR_DEFINE_IRQ	45	; Coprocessor/interprocessor
ISR_DEFINE_IRQ	46	; Primary ATA Hard disk
ISR_DEFINE_IRQ	47	; Secondary ATA Hard disk

