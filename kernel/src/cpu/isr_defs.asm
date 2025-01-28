[bits 64]

; size of the general-purpose registers needed for getting the syscall number
REGISTER_SIZE: equ 0x78
QUADWORD_SIZE: equ 0x08

%macro PUSHALL 0
    push rdi
    push rsi
    push rdx
    push rcx
    push rax
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POPALL 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rax
    pop rcx
    pop rdx
    pop rsi
    pop rdi
%endmacro

%macro SAVE_REGS_AND_CALL_HANDLER 1
    PUSHALL

    ; Configure arguments for the method call (Using SYSV ABI)
    ; RDI Should contain the interrupt number
    ; RSI Should contain the error code
    ; RDX Should contain the pointer to the registers (AKA the stack pointer)
    mov rdx, rsp
    mov rdi, [rsp + REGISTER_SIZE]                      ; ISR Number is last on the stack
    mov rsi, [rsp + REGISTER_SIZE + QUADWORD_SIZE]      ; Error Code is first on the stack

    call %1

    ; restore all registers before returning
    POPALL
%endmacro

%macro ISR_NOERRCODE 1
  isr_%1:

    push qword 0 ; no error
    push qword %1

    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
  isr_%1:

    push qword %1

    jmp isr_common
%endmacro

%macro ISR_NAME 1
  dq isr_%1
%endmacro

section .text
; jump to interrupt common
[extern isr_handler]

isr_common:
    SAVE_REGS_AND_CALL_HANDLER isr_handler
    add rsp, 0x10 ; pop the stack by 2 quadwords for the ISR Number and Error Code
    iretq
  
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
; I'm using the NOERRCODE macro to avoid a page fault when returning from this interrupt.
ISR_NOERRCODE 8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE 30
ISR_NOERRCODE 31

; loop from 32 to 255 for user-defined interrupts/ISRs
%assign i 32
%rep 224
  ISR_NOERRCODE i
%assign i i+1
%endrep

; place all the ISRs in an array
section .data
global isr_vector
isr_vector:
%assign i 0
%rep 256
  ISR_NAME i 
%assign i i+1
%endrep

