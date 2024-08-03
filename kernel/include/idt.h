#pragma once 

#include <macros.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#define IDT_SIZE            256 // 256 entries in the IDT for x86_64
#define IDT_TYPE_PRESENT    0b1
#define IDT_TYPE_PRIVILEGE  0b00
#define IDT_IST_ISR         1 
#define IDT_IST_IRQ         2
#define IDT_IST_TIMER       3

enum idt_gate_type {
	IDT_GATE_TYPE_INTERRUPT	= 0b1110,
	IDT_GATE_TYPE_TRAP		= 0b1111,
	IDT_GATE_TYPE_TASK		= 0b0101,
};

typedef struct s_idt_entry {
    uint16_t offset_lo;     /* offset 0-15 */ 
    uint16_t selector;      /* code segment selector in GDT or LDT */
    uint8_t ist;            /* bits 0-2 hold Interrupt Stack Table, remainding bits are 0 */ 
    uint8_t type_attr;      /* gate type, dpl, and p fields */
    uint16_t offset_mid;    /* offset 16-31 */ 
    uint32_t offset_hi;     /* offset 32-63 */
    uint32_t zero;          /* reserved */
} PACKED idt_entry_t;

typedef struct s_idt_descriptor {
    uint16_t limit; /* size */ 
    idt_entry_t *base; /* offset */
} PACKED idt_descriptor_t;

typedef struct s_idt_block {
    idt_descriptor_t idtd;
    idt_entry_t idt[IDT_SIZE];
} idt_block_t;

void idt_init(idt_block_t *);
void idt_set_isr(idt_block_t *, size_t, void *, enum idt_gate_type, uint8_t);
void idt_load(idt_descriptor_t *, bool);

