#ifndef __CPU_IDT
#define __CPU_IDT

#include <types.h>

#define KERNEL_CS   0x08 /* kernel code segment */
#define INT_ATTR    0x8E /* idt gate (entry) attribute: 0b10001110 */
#define IDT_ENTRIES 256

/* interrupt handler register */
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idtr_t;

/* interrupt handler gate */
typedef struct {
    uint16_t isr_low;       /* lower 16-bits of the addr to jump to */ 
    uint16_t cs_selector;   /* the GDT segment selector the CPU will load into CS before calling the ISR */
    uint8_t  ist;           /* the IST in the TSS that the CPU will load into RSP; set to zero for now */
    uint8_t  attributes;    /* flag bytes: 
                                bit 7:      interrupt is present
                                bits 6-5:    privilege level
                                bit 4:      set to 0 for interrupt gates
                                bits 3-0:   always 1110 for 32-bit interrupt gate */
    uint16_t isr_mid;       /* middle 16 bits of addr to jump to */
    uint32_t isr_hi;        /* higher 32 bits of addr to jump to */
    uint32_t reserved;      /* set to zero */
} __attribute__((packed)) idt_gate_t;

extern idt_gate_t main_idt[IDT_ENTRIES];
extern idtr_t main_idt_reg;

void set_idt();
void set_idt_gate(uint8_t vector, uint64_t isr_addr);

#endif /* ifndef __CPU_IDT */
