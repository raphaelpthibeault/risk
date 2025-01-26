#include <driver/serial.h>
#include <types.h>
#include <driver/vga.h>
#include <cpu/isr.h>
#include <cpu/pic.h>

int main() {
    if (serial_init() == 1) {
        while (1);
    }

    serial_write_str("serial writing\n"); // with qemu using "-serial mon:stdio" the printing stuff just works; just have to do variadic list print
    serial_write_str("foobar");

    isr_install(); // installs IDT and ISRs
    pic_disable(); // disable PIC, will be doing APIC

    set_cursor_pos(0, 0);
    clearwin(COLOR_WHT, COLOR_BLK);

    putstr("\nhello, world!,\n", COLOR_GRN, COLOR_BLK);

    return 0;
}

