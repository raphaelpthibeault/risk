#include <driver/vga.h>
#include <types.h>

int main() {
    set_cursor_pos(0, 0);
    clearwin(COLOR_WHT, COLOR_BLU);

    const char *foo = "\n\n\nfoo. The foo message as text to test the newline character.";
    putstr(foo, COLOR_WHT, COLOR_BLU);

    const char *bar = "\n\tbar. Bar should be tabbed";
    putstr(bar, COLOR_WHT, COLOR_BLU);

    const char *spam = "\nThis should scroll.";
    for (uint16_t i = 0; i < 21; ++i)
        putstr(spam, COLOR_WHT, COLOR_BLU);

    return 0;
}

