#pragma once 
#include <macros.h>
#include <stdint.h>
#include <stdbool.h>

#define IDT_ENTRY_COUNT 256

enum {
    IDT_TRAP = 0xef,
    IDT_USER = 0x60, // task
    IDT_GATE = 0x8e
};

/* see Section 6.14: Exception and Interrupt Handling in 64-bit Mode of the Intel Software Developer Manual, Volume 3-A */

typedef struct {
    uint16_t    offset_0_15;
    uint16_t    code_segment;
    uint8_t     ist;
    uint8_t     attributes;
    uint16_t    offset_16_32;
    uint32_t    offset_32_63;
    uint32_t    _zero;
} PACKED idt_entry_t;

typedef struct {
    uint16_t    limit;
    idt_entry_t *base;
} PACKED idtr_t;

typedef struct {
    idt_entry_t entries[IDT_ENTRY_COUNT];
} PACKED idt_t;

idt_entry_t idt_create_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags);
void idt_assemble(void);
void idt_reload(idtr_t *);

