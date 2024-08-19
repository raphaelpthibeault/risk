#include "memory/mmu.h"
#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <graphics/psf.h>
#include <multiboot.h>
#include <elf.h>

extern uint8_t _psf_font_version;

multiboot_t *multiboot_struct = NULL;

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
        printf("[fatal] incorrect mboot mag\r\n");
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

    /* multiboot2 initialization */
    multiboot2_addresses_t highest_addrs = multiboot2_init(mboot);
    printf("[info] highest valid address: %llu\r\n", highest_addrs.highest_valid_address);
    printf("[info] highest kernel address: %llu\r\n", highest_addrs.highest_kernel_address);

    /* mboot but higher mapped */
    multiboot_struct = mmu_map_from_physical((uintptr_t)mboot);

    /* TODO: initialize scheduler for bsp */
    /* TODO: initialize per-CPU kernel state for the bsp */
    // TODO: gdt_assemble (which has reload inside)
    // TODO: idt_assemble (which has reload inside)
    // TODO: framebuffer
}



