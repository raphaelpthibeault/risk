#include <idt.h>
#include <isr.h>
#include <gdt.h>
#include <stdint.h>
#include <mem.h>

#define IDT_SET_ISR(num)	idt_set_isr(idtb, num, isr##num, IDT_GATE_TYPE_TRAP, IDT_IST_ISR)
#define IDT_SET_IRQ(num)	idt_set_isr(idtb, num, isr##num, IDT_GATE_TYPE_INTERRUPT, IDT_IST_IRQ)

void 
idt_init(idt_block_t *idtb) {
    size_t idt_total_size = IDT_SIZE * sizeof(idt_entry_t);
    memset(&idtb->idt, 0, idt_total_size);
    idtb->idtd.limit = idt_total_size - 1;
    idtb->idtd.base = idtb->idt;

    // Internal ISRs
    IDT_SET_ISR(8);
    IDT_SET_ISR(10);
    IDT_SET_ISR(11);
    IDT_SET_ISR(13);
    IDT_SET_ISR(14); 

    // IRQs
    IDT_SET_IRQ(33);

}

void 
idt_set_isr(idt_block_t *idtb, size_t i, void *isrptr, enum idt_gate_type gate, uint8_t ist) {
    idt_entry_t *idt = idtb->idt;
    idt[i] = (idt_entry_t) {
		.zero = 0,
		.offset_lo = (uint16_t)(((uint64_t)isrptr & 0x000000000000ffff)),
		.offset_mid = (uint16_t)(((uint64_t)isrptr & 0x00000000ffff0000) >> 16),
		.offset_hi = (uint32_t)(((uint64_t)isrptr & 0xffffffff00000000) >> 32),
		.ist = ist & 0x7,
		.selector = 1 * sizeof(gdt_entry_t),	
		.type_attr = IDT_TYPE_PRESENT << 7 | IDT_TYPE_PRIVILEGE << 5 | gate
    };
}

