#include "interrupts.h"
#include <apic/apic.h>
#include <stdint.h>
#include <cpu/cpu.h>
#include <lib/asm/asm.h>
#include <lib/std/printf.h>

/* private stackframe structure per Section 6.14.2: 64-Bit Mode Stack Frame of the Intel Software Developer Manual, Volume 3-A */
typedef struct s_stackframe {
    struct s_stackframe *rbp;
    uint64_t rip;
} stackframe_t;

static void registers_dump(registers_t const *regs);
static uint64_t backtrace_dump(uintptr_t rbp);

/* see Section 6.15: Exception and Interrupt Reference of the Intel Software Developer Manual, Volume 3-A
 * Interrupts 0-21 are per Intel 
 * the rest are user defined
 * */
static char *_error_messages[32] = {
    /* Intel defined */
    "DivisionByZero",
    "Debug",
    "NonMaskableInterrupt",
    "Breakpoint",
    "DetectedOverflow",
    "OutOfBounds",
    "InvalidOpcode",
    "NoCoprocessor",
    "DoubleFault",
    "CoprocessorSegmentOverrun",
    "BadTss",
    "SegmentNotPresent",
    "StackFault",
    "GeneralProtectionFault",
    "PageFault",
    "UnknownInterrupt",
    "CoprocessorFault",
    "AlignmentCheck",
    "MachineCheck",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    /* user defined */
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

static 
void 
interrupt_error_handler(registers_t *regs) {
    printf("[error] error {%llu : %s} triggered\r\n", regs->int_num, _error_messages[regs->int_num]);

    registers_dump(regs);
    backtrace_dump(regs->rbp);

    while (true) {
        cli(); 
        hlt();
    }
}

uint64_t 
interrupt_handler(uint64_t rsp) {
    registers_t *regs = (registers_t*)rsp;
    if (regs->int_num < 32) {
        interrupt_error_handler(regs);
    }
    
    cpu_being_interrupt();
    if (regs->int_num >= 32 && regs->int_num <= 48) {
        printf("[info] event {%llu} triggered!\r\n", regs->int_num);
        /* TODO
         * triggers event
         * needs tasking and scheduling I think
         * */
    }

    switch (regs->int_num) {
        case 32: {
            /* TODO
             * tasking
             * scheduling 
             * */
            break;
        }
        case IPI_RESCHEDULE: {
            /* TODO
             * tasking
             * scheduling 
             * */
            break;
        }
        case IPI_STOP: {
            while (true) {
                cli(); 
                hlt();
            }
            break;
        }
        case 0xf0: {
            printf("[debug] non-maskable interrupt from APIC. Possible hardware error.\r\n");
            break;
        }
    }

    cpu_end_interrupt();
    lapic_eoi();

    return rsp;
}

/* privates */

static 
void 
registers_dump(registers_t const *regs) {
    printf("[debug] registers dump:\r\n");
    printf("RIP: {%llu}, RSP: {%llu}\r\n", regs->rip, regs->rsp); 
    printf("CS: {%llu}, SS: {%llu}\r\n", regs->cs, regs->ss); 
    printf("RAX: {%llu}, RBX: {%llu}\r\n", regs->rax, regs->rbx); 
    printf("RCX: {%llu}, RDX: {%llu}\r\n", regs->rcx, regs->rdx); 
    printf("RSI: {%llu}, RDI: {%llu}\r\n", regs->rsi, regs->rdi); 
    printf("RBP: {%llu}, R8: {%llu}\r\n", regs->rbp, regs->r8); 
    printf("R9: {%llu}, R10: {%llu}\r\n", regs->r9, regs->r10); 
    printf("R11: {%llu}, R12: {%llu}\r\n", regs->r11, regs->r12); 
    printf("R13: {%llu}, R14: {%llu}\r\n", regs->r13, regs->r14); 
    printf("R15: {%llu}\r\n", regs->r15); 
}


static 
uint64_t 
backtrace_dump(uintptr_t rbp) {
    stackframe_t *stackframe = (stackframe_t*)rbp; 
    uint64_t i = 0;

    printf("[debug] backtrace dump:\r\n");
    while (stackframe) {
        printf("- {%p}\r\n", stackframe->rip);
        stackframe = stackframe->rbp;
    }

    return i;
}

