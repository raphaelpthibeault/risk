#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#define ISR_IRQ_START 32 
#define ISR_MAX 256 // theoretical max of 256

/* Interrupt Service Routines */ 

/*
 * Exception and Interrupt Request (IRQ) Vectors
 * see Intel Volume 3, Chapter 6 Section 2, Table 6-1
 *
 * Exceptions:
 * Vector  0:  Divide By Zero Exception
 * Vector  1:  Debug Exception
 * Vector  2:  Non Maskable Interrupt Exception
 * Vector  3:  Breakpoint Exception
 * Vector  4:  Into Detected Overflow Exception
 * Vector  5:  Out of Bounds Exception
 * Vector  6:  Invalid Opcode Exception
 * Vector  7:  No Coprocessor Exception
 * Vector  8:  Double Fault Exception (with error code)
 * Vector  9:  Coprocessor Segment Overrun Exception
 * Vector 10:  Invalid Task State Segment (with error code)
 * Vector 11:  Segment Not Present Exception (with error code)
 * Vector 12:  Stack Fault Exception (with error code)
 * Vector 13:  General Protection Fault Exception (with error code)
 * Vector 14:  Page Fault Exception (with error code)
 * Vector 16:  Coprocessor Fault Exception
 * Vector 17:  Alignment Check Exception (486+)
 * Vector 18:  Machine Check Exception (Pentium/586+)
 * Vector 19:  SIMD Floating-Point Exception
 * Vector 20:  Virtualization Exception
 * Vector 21:  Control Protection Exception
 * Vector 30:  Security Exception (with error code)
 *
 * Interrupt Requests (IRQs):
 * Vector 32:  PIT (Programmable Interval Timer)
 * Vector 33:  Keyboard Interrupt
 * Vector 34:  Cascade
 * Vector 35:  COM2 (Serial Port 2)
 * Vector 36:  COM1 (Serial Port 1)
 * Vector 37:  LPT2 (Parallel Port 2)
 * Vector 38:  Floppy Disk
 * Vector 39:  LPT1 (Parallel Port 1, spurious interrupt)
 * Vector 40:  CMOS (Real-Time Clock)
 * Vector 41:  Peripherals 1 (legacy SCSI/NIC)
 * Vector 42:  Peripherals 2 (SCSI/NIC)
 * Vector 43:  Peripherals 3 (SCSI/NIC)
 * Vector 44:  PS2 Mouse
 * Vector 45:  Coprocessor/Interprocessor
 * Vector 46:  Primary ATA Hard Disk
 * Vector 47:  Secondary ATA Hard Disk
 *
 * Custom Local Vector Table (LVT) IRQs:
 * Vector 48:  Timer (Direct)
 */

typedef struct s_interrupt_general_registers {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15;
} interrupt_general_registers_t;

typedef struct s_interrupt_stack {
    interrupt_general_registers_t gen;
    uint64_t rip, cs, rflags, rsp, ss;
} interrupt_stack_t;

typedef struct s_interrupt_error_stack {
    interrupt_general_registers_t gen;
    uint64_t error_code, rip, cs, rflags, rsp, ss;
} interrupt_error_stack_t;

/* global ISR handler table */ 
extern void (*g_isr_handlers[ISR_MAX])(interrupt_stack_t*, size_t);

/* registers all default ISR handlers in g_isr_handlers */ 
void isr_init();

/* adds ISR handler for interrupt # in g_isr_handlers */
void isr_register_handler(size_t, void (*)(interrupt_stack_t*, size_t));

/* returns true if isr for # has been registered in the table */
bool isr_exists(size_t);


/* ISRs defined at the top of the page, implementation in isr.asm */ 
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr30();

extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

extern void isr48();

