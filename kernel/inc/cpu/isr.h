#ifndef __CPU_ISR
#define __CPU_ISR

#include <types.h>

typedef struct {
    // callee-saved registers
    uint64_t r15, r14, r13, r12, rbp, rbx;
    // callee-clobbered registers
    uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;
    // IRQ num and error code
    uint64_t irq_num, error_code;
    // return frame for iretq call
    uint64_t rip, cs, eflags, ss;
} __attribute__((packed)) registers_t;

void isr_install();
void isr_handler(uint64_t isr_num, uint64_t error_code, registers_t *regs); 

#endif /* ifndef __CPU_ISR */
