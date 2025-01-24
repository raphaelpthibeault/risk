#include <types.h>
#include <driver/vga.h>
#include <cpu/isr.h>

int main() {
    isr_install(); 

    set_cursor_pos(0, 0);
    clearwin(COLOR_WHT, COLOR_BLK);

    putstr("\nThis interrupt is most likely NOT a double-fault,\n", COLOR_GRN, COLOR_BLK);
    putstr("but a problem with not remapping the timer IRQ from the PIC,\n", COLOR_GRN, COLOR_BLK);
    putstr("so it shows up on gate 8\n", COLOR_GRN, COLOR_BLK);

    return 0;
}

