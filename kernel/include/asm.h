#pragma once

// Halt and catch fire function
static inline void 
hcf(void) {
    for (;;) {
#if defined (__x86_64__)
        asm ("hlt");
#elif defined (__aarch64__) || defined (__riscv)
        asm ("wfi");
#endif
    }
}

static inline void 
disable_interrupts(void) {
#ifdef __x86_64__
    asm volatile ("cli");
#elif defined(__aarch64__)
    asm volatile ("msr daifset, #0x3");
#elif defined(__riscv)
    asm volatile ("csrrc x0, mstatus, %0" : : "r"(1 << 3));
#else
#error "Unsupported architecture"
#endif
}

static inline void
enable_interrupts(void) { 
#ifdef __x86_64__
    asm volatile ("sti");
#elif defined(__aarch64__)
    asm volatile ("msr daifclr, #0x3");
#elif defined(__riscv)
    asm volatile ("csrrs x0, mstatus, %0" : : "r"(1 << 3));
#else
#error "Unsupported architecture"
#endif
}
