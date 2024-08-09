#include "gdt.h" 
#include <lib/sync/lock.h>
#include <stdint.h>
#include <lib/std/printf.h>

static tss_t _tss = {
    .reserved = 0,
    .rsp = {},
    .reserved0 = 0,
    .ist = {},
    .reserved1 = 0,
    .reserved2 = 0,
    .reserved3 = 0
};

static lock_t _lock;
static gdt_t _gdt = {};
static gdtr_t _gdtr;

gdt_segment_descriptor_t 
gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags) {
    return (gdt_segment_descriptor_t) {
        .limit_0_15 = (uint16_t)(limit & 0xffff),
        .base_0_15 = (uint16_t)(base & 0xffff),
        .base_16_23 = (uint8_t)((base >> 16) & 0xff),
        .flags = flags,
        .limit_16_19 = (uint8_t)((limit >> 16) & 0x0f),
        .granularity = granularity,
        .base_24_31 = (uint8_t)((base >> 24) & 0xff)
    };
}

gdt_segment_descriptor_t 
gdt_create_null_entry(void) {
    return gdt_create_entry(0, 0, 0, 0);
}

gdt_segment_descriptor_t 
gdt_create_entry_from_flags(uint8_t flags, uint8_t granularity) {
    return gdt_create_entry(0, 0, granularity, flags);
}

gdt_tss_segment_descriptor_t 
gdt_create_tss_entry(uintptr_t tss) {
    return (gdt_tss_segment_descriptor_t) {
        .length = sizeof(gdt_tss_segment_descriptor_t),
        .base_0_15 = tss & 0xffff,
        .base_16_23 = (tss >> 16) & 0xff,
        .flags1 = 0b10001001,
        .base_24_31 = (tss >> 24) & 0xff,
        .flags2 = 0,
        .base_32_63 = tss >> 32,
        .reserved = 0
    };
}

void 
gdt_assemble(void) {
    printf("[info] assembling gdt\r\n");

    _gdt.entries[0] = gdt_create_null_entry();

    _gdt.entries[GDT_KERNEL_CODE] = gdt_create_entry_from_flags(GDT_BASIC_DESCRIPTOR | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY);
    _gdt.entries[GDT_KERNEL_DATA] = gdt_create_entry_from_flags(GDT_BASIC_DESCRIPTOR, 0); 
    
    _gdt.entries[GDT_USER_CODE] = gdt_create_entry_from_flags(GDT_BASIC_DESCRIPTOR | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY);
    _gdt.entries[GDT_USER_DATA] = gdt_create_entry_from_flags(GDT_BASIC_DESCRIPTOR | GDT_USER, 0); 

    _gdt.tss = gdt_create_tss_entry((uintptr_t)&_tss);

    printf("[info] reloading gdt... %llu \r\n", (uintptr_t)&_gdt);
    _gdtr.size = GDT_DESCRIPTOR_COUNT * sizeof(gdt_segment_descriptor_t) - 1;
    _gdtr.offset = _gdt.entries;

    gdt_reload(&_gdtr);
    printf("[info} gdt assembled!\r\n");
}

void 
gdt_install_tss(tss_t *tss) {
    printf("[info] installing tss into gdt...\r\n");
    lock_acquire(&_lock);

    _gdt.tss = gdt_create_tss_entry((uintptr_t)tss);
    tss_reload();

    lock_release(&_lock);
    printf("[info] tss installed!\r\n");
}

void 
gdt_dump() {
    printf("[debug] GDT DUMP: \r\n");
    for (uint16_t i = 0; i < GDT_DESCRIPTOR_COUNT; ++i) {
        gdt_segment_descriptor_t *descriptor = &_gdt.entries[i];
        uint64_t base = (uint64_t)descriptor->base_0_15 | ((uint64_t)descriptor->base_16_23 << 16) | ((uint64_t)descriptor->base_24_31 << 32);
        uint32_t limit = (uint32_t)descriptor->limit_0_15 | ((uint32_t)descriptor->base_16_23 << 16);
        printf("\t%d => lo: %x, hi: %x, access: %x, gran: %x\r\n", (i<2)?"0":"", i*GDT_DESCRIPTOR_SIZE, base, limit, descriptor->flags, descriptor->granularity);
    }
}

