#ifndef __CPU_PORTS
#define __CPU_PORTS

#include <types.h>

/* read a byte from a port */
uint8_t inb(uint16_t p);

/* write a byte to a port */
void outb(uint16_t p, uint8_t b);

#endif /* ifndef __CPU_PORTS */
