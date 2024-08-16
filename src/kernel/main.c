#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <graphics/psf.h>

static void _init_system(uint64_t addr);


extern uint8_t _psf_font_version;

void 
kernel_start(uint64_t addr, uint64_t magic) {
    serial_init_write();
    serial_write_str("\r\n", SERIAL_COM1_PORT);
    serial_write_str(
    "........      ...   ......   ..    ..\r\n"
    "..     ..    . .   ..        ..   ..\r\n"
    "..      ..   . .  ..         ..  ..\r\n"
    "..     ..    . .   ..        .. ..\r\n"
    "..   ..      . .    .....    ....\r\n"
    "..  ..       . .        ..   .. ..\r\n"
    "..    ..     . .         ..  ..  ..\r\n"
    "..     ..    . .        ..   ..   ..\r\n"
    "..      ..  ...    ......    ..    ..\r\n", SERIAL_COM1_PORT);


    if (magic != 0x36d76289) {
        printf("[error] invalid magic\r\n");
        while (1);
    }

    _psf_font_version =  psf_get_version(_binary_fonts_default_psf_start);
    printf("PSF font version: %d\r\n", _psf_font_version);

    // gdt_assemble (which has reload inside)
    // idt_assemble (which has reload inside)
    // framebuffer

    _init_system(addr);

}


static void 
_init_system(uint64_t addr) {
    printf("_init_system ( %llu )\r\n", addr);
    // memory

}

