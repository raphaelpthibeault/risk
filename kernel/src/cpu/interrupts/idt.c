#include "idt.h"
#include <stdbool.h>
#include <stdint.h>
#include <cpu/gdt/gdt.h>
#include <apic/apic.h>
#include <cpu/cpu.h>
#include <lib/std/printf.h>

extern uintptr_t _interrupt_vector[IDT_ENTRY_COUNT];

static idt_t _idt = {};
static idtr_t _idtr;


idt_entry_t 
idt_create_entry(uintptr_t handler, uint8_t ist, uint8_t idt_flags) {
    return (idt_entry_t) {
        .attributes = idt_flags,
        .ist = ist,
        .offset_0_16 = handler,
        .offset_16_32 = (handler >> 16),
        .offset_32_63 = (handler >> 32),
        .code_segment = GDT_KERNEL_CODE * GDT_DESCRIPTOR_SIZE,
        ._zero = 0
    };
}

void 
idt_assemble(void) {
    printf("[info] assembling idt\r\n");
    uint32_t i;

    for (i = 0; i < IDT_ENTRY_COUNT; ++i) {
        _idt.entries[i] = idt_create_entry(_interrupt_vector[i], 0, IDT_GATE);
    }

    // special case for timer IRQ
    _idt.entries[IPI_STOP] = idt_create_entry(_interrupt_vector[IPI_STOP], 1, IDT_GATE);

    _idtr.limit = IDT_ENTRY_COUNT * sizeof(idt_entry_t) - 1;
    _idtr.base = _idt.entries;

    printf("[info] reloading idt...\r\n");
    idt_reload(&_idtr);
    printf("[info] idt assembled!\r\n");
}

