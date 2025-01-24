[bits 64]
[extern main]

section .startup
global _start
_start:
    call main
    jmp $

