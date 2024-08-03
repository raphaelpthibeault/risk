%include    "gdt.inc"

SECTION     .text
GLOBAL      gdt_load
; convetion: rdi should contain the gdt descriptor
gdt_load: 
    ; setup GDT
    cli
    lgdt    [rdi]           ; load global descriptor table
    ; setup TSS
    mov     ax, OFFSET_TSS_SEG
    ltr     ax ; load task register
	; This will exploit iretq's ability to set segments in long mode
	; The order iretq will pop is: RIP, CS, RFLAGS, RSP, SS
	; We first need to save data we wish to keep
    pop     rsi ; keep return address in rsi
    mov     rdi, rsp ; keep old stack pointer in rdi
    mov     rax, OFFSET_KERNEL_DATA_SEG
    mov     ds, ax 
    mov     es, ax 
    ; push what iretq needs 
    push    rax ; push stack segment
    push    rdi ; pushold stack pointer
    pushfq ; push flags
    push    QWORD OFFSET_KERNEL_CODE_SEG
    push    rsi
    iretq

