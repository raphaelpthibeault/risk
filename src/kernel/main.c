#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <graphics/psf.h>
#include <multiboot.h>
#include <elf.h>

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
        while (1); // halt; TODO: panic function
    }

    /* elf relocations */
    extern Elf64_Rela _rela_start[], _rela_end[];
    for (Elf64_Rela *rela = _rela_start; rela < _rela_end; ++rela) {
        switch (ELF64_R_TYPE(rela->r_info)) {
            case R_X86_64_RELATIVE:
                *(uint64_t*)(rela->r_offset + base) = base + rela->r_addend;
                break;
        }
    }

    /* TODO: multiboot2 */
    /* TODO: initialize scheduler for bsp */
    /* TODO: initialize per-CPU kernel state for the bsp */
    // TODO: gdt_assemble (which has reload inside)
    // TODO: idt_assemble (which has reload inside)
    // TODO: framebuffer
}

