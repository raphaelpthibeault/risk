#include <cpu/ports.h>
#include <types.h>

uint8_t inb(uint16_t p) {
    /*
        format of asm inline instructions is "command" : "output" : "input"
        "in" is used to read from a port
        "=a" (res) means put value of rax in result
        "d" (port) means put value of port in dx
     */

    uint8_t res;
    __asm__ volatile ("in %%dx, %%al" : "=a" (res) : "d" (p));
    return res;
}

void outb(uint16_t p, uint8_t b) {
   __asm__ volatile ("out %%al, %%dx" : : "a" (b), "d" (p));  
}

