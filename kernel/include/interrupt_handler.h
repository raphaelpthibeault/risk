#pragma once 

/* handles interrupts and dispatches events to userspace */
/* TODO: add all the rest */

#include <isr.h>

/* EXCEPTIONS */

// ISR 8
void double_fault_handler(interrupt_stack_t *, size_t);
// ISR 10
void tss_fault_handler(interrupt_stack_t *, size_t);
// ISR 11
void segment_fault_handler(interrupt_stack_t *, size_t);
// ISR 13
void general_protection_fault_handler(interrupt_stack_t *, size_t);
// ISR 14
void paging_fault_handler(interrupt_stack_t *, size_t);


/* IRQS */
// ISR 33
void keyboard_handler(interrupt_stack_t *, size_t);

