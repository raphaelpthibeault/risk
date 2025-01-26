#include <cpu/pic.h>
#include <types.h>
#include <cpu/ports.h>

#define PIC1		0x20	    /* IO base address for master PIC */
#define PIC1_COMMAND    PIC1
#define PIC1_DATA	(PIC1+1)

#define PIC2		0xA0	    /* IO base address for slave PIC */
#define PIC2_COMMAND    PIC2
#define PIC2_DATA	(PIC2+1)


void pic_disable() {
   outb(PIC1_COMMAND, 0x11);
   outb(PIC2_COMMAND, 0x11);

   outb(PIC1_DATA, 0x20);
   outb(PIC2_DATA, 0x28);

   outb(PIC1_DATA, 0x2);
   outb(PIC2_DATA, 0x4);

   outb(PIC1_DATA, 0x01);
   outb(PIC2_DATA, 0x01);

   outb(PIC1_DATA, 0xFF);
   outb(PIC2_DATA, 0xFF);
}

