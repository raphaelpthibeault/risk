#pragma once

// Halt and catch fire function
static inline void
hcf(void) {
    for (;;) {
#if defined (__x86_64__)
        __asm__ ("hlt");
#elif defined (__aarch64__) || defined (__riscv)
        __asm__ ("wfi");
#endif
    }
}

static inline void
disable_interrupts(void) {
#ifdef __x86_64__
    __asm__ volatile ("cli");
#elif defined(__aarch64__)
    __asm__ volatile ("msr daifset, #0x3");
#elif defined(__riscv)
    __asm__ volatile ("csrrc x0, mstatus, %0" : : "r"(1 << 3));
#else
#error "Unsupported architecture"
#endif
}

static inline void
enable_interrupts(void) {
#ifdef __x86_64__
    __asm__ volatile ("sti");
#elif defined(__aarch64__)
    __asm__ volatile ("msr daifclr, #0x3");
#elif defined(__riscv)
    __asm__ volatile ("csrrs x0, mstatus, %0" : : "r"(1 << 3));
#else
#error "Unsupported architecture"
#endif
}
