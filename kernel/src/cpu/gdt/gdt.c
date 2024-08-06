#include "gdt.h"
#include <mm/paging/paging.h>
#include <lib/std/printf.h>

PAGING_PAGE_ALIGNED 
gdt_desc_t _gdt[GDT_MAX_DESCRIPTORS];

static gdtr_t gdtr;
static uint16_t gindex;

void 
gdt_assemble() {
    gdtr.limit = (sizeof(gdt_desc_t) * GDT_MAX_DESCRIPTORS) - 1;
    gdtr.base = (uintptr_t)&_gdt[0];

    gdt_add_descriptor(0, 0, 0, 0); // null descriptor for segment registers to have offsets
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY); // kernel code
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR, GDT_BASIC_GRANULARITY); // kernel data
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL, GDT_BASIC_GRANULARITY); // user data
    gdt_add_descriptor(0, 0xFFFF, GDT_BASIC_DESCRIPTOR | GDT_DESCRIPTOR_DPL | GDT_DESCRIPTOR_EXECUTABLE, GDT_BASIC_GRANULARITY); // user code
    gdt_add_descriptor(0, 0, 0, 0); // null descriptor delimiting the TSS

    gdt_reload(&gdtr, GDT_OFFSET_KERNEL_CODE, GDT_OFFSET_KERNEL_DATA);
}

void gdt_add_descriptor(uint64_t base, uint16_t limit, uint8_t access, uint8_t granularity) {
    if (gindex >= GDT_MAX_DESCRIPTORS) 
        return;

    _gdt[gindex].base_low = base & 0xFFFF;
    _gdt[gindex].base_mid = (base >> 16) & 0xFF;
    _gdt[gindex].base_high = (base >> 24) & 0xFF;

    _gdt[gindex].limit = limit;

    _gdt[gindex].flags = access;
    _gdt[gindex].granularity = granularity;

    gindex++;
}

#define TSS_SIZE 0x70

uint16_t 
gdt_install_tss(uint64_t tss) {
    uint8_t tss_type = GDT_DESCRIPTOR_ACCESS | GDT_DESCRIPTOR_EXECUTABLE | GDT_DESCRIPTOR_PRESENT;

    gdt_tss_desc_t* tss_desc = (gdt_tss_desc_t *)&_gdt[gindex];

    if (gindex >= GDT_MAX_DESCRIPTORS)
        return 0;

    tss_desc->limit_0 = TSS_SIZE & 0xFFFF;
    tss_desc->addr_0 = tss & 0xFFFF;
    tss_desc->addr_1 = (tss & 0xFF0000) >> 16;
    tss_desc->type_0 = tss_type;
    tss_desc->limit_1 = (TSS_SIZE & 0xF0000) >> 16;
    tss_desc->addr_2 = (tss & 0xFF000000) >> 24;
    tss_desc->addr_3 = tss >> 32;
    tss_desc->reserved = 0;

    gindex += 2;
    return (gindex - 2) * GDT_DESCRIPTOR_SIZE;
}

void 
__uart_gdt_dump() {
    printf("GDT dump: gindex: %d\r\n", gindex);
    for (uint16_t i = 0; i < gindex; i++) {
        gdt_desc_t* descriptor = &_gdt[i];
        uint64_t base = (uint64_t)descriptor->base_low | ((uint64_t)descriptor->base_mid << 16) | ((uint64_t)descriptor->base_high << 32);
        printf("Descriptor %llu => \r\n", i * GDT_DESCRIPTOR_SIZE);
        printf("\tlo: %llu\r\n", base);
        printf("\thi: %llu\r\n", descriptor->limit);
        printf("\taccess: %llu\r\n", descriptor->flags);
        printf("\tgran: %llu\r\n", descriptor->granularity);
    }
}

