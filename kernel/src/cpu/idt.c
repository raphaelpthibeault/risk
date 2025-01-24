#include <cpu/idt.h>
#include <stdint.h>

idt_gate_t main_idt[IDT_ENTRIES];
idtr_t main_idt_reg;

void set_idt() {
   main_idt_reg.limit = (IDT_ENTRIES * sizeof(idt_gate_t)) - 1;
   main_idt_reg.base = (uint64_t) &main_idt;
   
   // load the value of the pointer to the idt register
   __asm__ volatile ("lidt (%0)" : : "r" (&main_idt_reg));

}

void set_idt_gate(uint8_t gate_num, uint64_t isr_addr) {
   idt_gate_t gate = {
      .isr_low = (uint16_t) (isr_addr & 0xFFFF),
      .cs_selector = KERNEL_CS,
      .ist = 0,
      .attributes = INT_ATTR,
      .isr_mid = (uint16_t) ((isr_addr >> 16) & 0xFFFF),
      .isr_hi = (uint32_t) ((isr_addr >> 32) & 0xFFFFFFFF),
      .reserved = 0
   };

   main_idt[gate_num] = gate;
}

