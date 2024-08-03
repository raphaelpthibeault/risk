#include <gdt.h>
#include <mem.h>
#include <stdint.h>


void
gdt_init(gdt_block_t *gdtb) {
    uint16_t gdt_total_size = GDT_NUM * sizeof(gdt_entry_t);
    memset(gdtb->tss, 0, GDT_TSS_NUM * sizeof(tss_entry_t)); 

    // set the gdt entries
    memset(gdtb->gdt, 0, gdt_total_size);                               // implicit definition of the entry 0, which is always null
    set_gdt_entry(gdtb, 1, GDT_ACCESS_EXECUTABLE, 0);                   // kernel code segment
    set_gdt_entry(gdtb, 2, GDT_ACCESS_WRITABLE, 0);                     // kernel data segment
    set_gdt_entry(gdtb, 3, GDT_ACCESS_DPL | GDT_ACCESS_EXECUTABLE, 0);  // user code segment 
    set_gdt_entry(gdtb, 4, GDT_ACCESS_DPL | GDT_ACCESS_WRITABLE, 0);    // user data segment
    set_gdt_tss(gdtb, 6);                                               // task state segment
    // set the descriptor
    gdtb->gdtd.size = gdt_total_size - 1; // recall, - 1
    gdtb->gdtd.offset = gdtb->gdt;
}

void 
set_gdt_entry(gdt_block_t *gdtb, size_t i, enum gdt_access access, enum gdt_flags flags) {
    gdtb->gdt[i].limit_lo = 0xffff;
    gdtb->gdt[i].access = GDT_ACCESS_TYPE | GDT_ACCESS_PRESENT | access;
    gdtb->gdt[i].flags = GDT_FLAG_LONG_MODE | GDT_FLAG_PAGE_GRANULARITY | flags;
}

void 
set_gdt_tss(gdt_block_t *gdtb, size_t gdt_index) {
    size_t tss_index = 0;
    tss_entry_t *tss = (tss_entry_t *)(gdtb->tss + gdt_index);
    *tss = (tss_entry_t) { .io_map_base = 0xffff };

    tss_descriptor_t *tssd = (tss_descriptor_t *)(gdtb->gdt + gdt_index);
    *tssd = (tss_descriptor_t){
        .limit_lo = sizeof(tss_entry_t),
        .base_lo = (uint64_t)tss & 0xffff,
        .base_mid = ((uint64_t)tss >> 16) & 0xff,
		.base_mid2 = ((uint64_t)tss >> 24) & 0xff,
		.base_hi = ((uint64_t)tss >> 32) & 0xffffffff,
		.access = 0b10000000 | 0b00001001,	// present, type (Intel Manual 3A 3.4.5.1) Execute-Only, accessed
		.flags = 0b00010000					// available
    };
}


