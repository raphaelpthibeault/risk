#pragma once 
#include <macros.h>
#include <stdint.h>

#define GDT_DESCRIPTOR_SIZE         0x08    /* 8 bytes long */
#define GDT_MAX_DESCRIPTORS         0x2000  /* a GDT can be up to 65536 bytes in length (which is 8192 entries = 0x2000) */
#define GDT_DESCRIPTOR_COUNT        5       /* only 5 for now */

#define GDT_KERNEL_CODE 0x01
#define GDT_KERNEL_DATA 0x02
#define GDT_USER_CODE   0x03
#define GDT_USER_DATA   0x04

#define GDT_SEGMENT                 0x10
#define GDT_PRESENT                 0x80
#define GDT_USER                    0x60
#define GDT_EXECUTABLE              0x08
#define GDT_READWRITE               0x02
#define GDT_LONG_MODE_GRANULARITY   0x02
#define GDT_FLAGS                   0xc

#define GDT_BASIC_DESCRIPTOR        (GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE)

/* see Section 3.4.5: Segment Descriptors and Figure 3-8: Segment Descriptor of the Intel Software Developer Manual, Volume 3-A. */
typedef struct {
    uint16_t    limit_0_15;     /* 0-15 */
    uint16_t    base_0_15;      /* 16-31 */
    uint8_t     base_16_23;     /* 32-39 */
    uint8_t     flags;          /* 40-47 */
    uint8_t     limit_16_19:4;  /* 48-51 */ 
    uint8_t     granularity:4;  /* 52-55 */ 
    uint8_t     base_24_31;     /* 56-63 */
} PACKED gdt_segment_descriptor_t;

/* see Section 8.2.3: TSS Descriptor in 64-bit Mode and Figure 8-4: Format of TSS and LDT Descriptors in 64-bit Mode of the Intel Software Developer Manual, Volume 3-A
 * though my implementation is only partly based off of that. 
 * */
typedef struct {
    uint16_t length;
    uint16_t base_0_15;
    uint16_t base_16_23;
    uint8_t flags1;
    uint8_t base_24_31;
    uint8_t flags2;
    uint32_t base_32_63;
    uint32_t reserved;
} PACKED gdt_tss_segment_descriptor_t;

typedef struct {
    gdt_segment_descriptor_t entries[GDT_DESCRIPTOR_COUNT];
    gdt_tss_segment_descriptor_t tss; // might want more than 1 tss eventually
} PACKED gdt_t;

typedef struct {
    uint16_t size;
    gdt_segment_descriptor_t* offset;
} PACKED gdtr_t;


typedef struct {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} PACKED tss_t;

void tss_reload(void);

void gdt_assemble(void);
void gdt_reload(gdtr_t *);
void gdt_install_tss(tss_t *tss);
void gdt_dump();

gdt_segment_descriptor_t gdt_create_null_entry(void);
gdt_segment_descriptor_t gdt_create_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags);
gdt_segment_descriptor_t gdt_create_entry_from_flags(uint8_t flags, uint8_t granularity);
gdt_tss_segment_descriptor_t gdt_create_tss_entry(uintptr_t tss);

