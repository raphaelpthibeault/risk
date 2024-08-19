#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <graphics/psf.h>
#include <multiboot.h>

extern uint8_t _psf_font_version;

/* x86_64 multiboot2 C entry point. Called by the x86_64 longmode bootstrap in boot.s
 * */
void 
kernel_start(multiboot_t *mboot, uint32_t mboot_mag, void *esp, uint64_t base) {
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

    if (mboot_mag != 920085129) {
        printf("[ERROR] incorrect mboot mag!\r\n");
    }

    /* elf rela */


    // TODO: gdt_assemble (which has reload inside)
    // TODO: idt_assemble (which has reload inside)
    // TODO: framebuffer


}


