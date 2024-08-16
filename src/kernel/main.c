#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <graphics/psf.h>

static void _init_system(uint64_t addr);


extern uint8_t _psf_font_version;

/* x86_64 multiboot2 C entry point. Called by the x86_64 longmode bootstrap in boot.s
 * */
void 
kernel_start(uint64_t addr, uint64_t magic) {
    serial_init_write();

    printf( "........      ...   ......   ..    ..\r\n"
            "..     ..    . .   ..        ..   ..\r\n"
            "..      ..   . .  ..         ..  ..\r\n"
            "..     ..    . .   ..        .. ..\r\n"
            "..   ..      . .    .....    ....\r\n"
            "..  ..       . .        ..   .. ..\r\n"
            "..    ..     . .         ..  ..  ..\r\n"
            "..     ..    . .        ..   ..   ..\r\n"
            "..      ..  ...    ......    ..    ..\r\n");
    printf("\r\n\r\n");

    if (magic != 0x36d76289) {
        printf("[error] invalid magic\r\n");
        while (1);
    }


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

