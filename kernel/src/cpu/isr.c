#include <types.h>
#include <cpu/isr.h>
#include <cpu/idt.h>
#include <driver/vga.h>

char *exception_messages[] = {
    "Division by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bat TSS",
    "Segment not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern uintptr_t isr_vector[256];

void isr_install(){
    uint32_t i;

    for (i = 0; i < 256; ++i) {
        set_idt_gate(i, isr_vector[i], 0);
    }

    // load the IDT to the CPU
    set_idt();

    // enable interrupts
    __asm__ volatile("sti");
}

__attribute__((sysv_abi))
void isr_handler(uint64_t isr_num, uint64_t error_code, registers_t *regs) {
    const char *msg = exception_messages[isr_num];
    putstr(msg, COLOR_WHT, COLOR_RED);
}

