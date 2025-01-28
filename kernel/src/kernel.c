#include <driver/serial.h>
#include <types.h>
#include <driver/vga.h>
#include <cpu/isr.h>
#include <cpu/pic.h>
#include <cpu/lapic.h>

int main() {
    if (serial_init() == 1) {
        while (1);
    }

    isr_install(); // installs IDT and ISRs
    serial_write_str("[Info] Installed IDT\n");
    pic_disable(); // disable PIC, will be doing APIC
    lapic_enable(); 

    set_cursor_pos(0, 0);
    clearwin(COLOR_WHT, COLOR_BLK);

    putstr("\nhello, world!,\n", COLOR_GRN, COLOR_BLK);

    return 0;
}

