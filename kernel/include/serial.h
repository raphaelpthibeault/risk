#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>


#ifdef __x86_64
#define PORT 0x3f8          // COM1
#endif
// riscv64 should be 0x10000000, aarch64 idk. A problem for later

int init_serial();
void write_serial(char a);
void write_serial_str(const char *str);

static inline void 
outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t 
inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

