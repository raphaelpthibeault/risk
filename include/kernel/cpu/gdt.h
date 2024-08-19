#ifndef GDT_H
#define GDT_H

#include <macros.h>
#include <stdint.h>

/* 64-bit TSS 
 * */
typedef struct {
	uint32_t reserved_0;
	uint64_t rsp[3];
	uint64_t reserved_1;
    uint64_t reserved_2;
	uint64_t ist[7];
	uint64_t reserved_3;
	uint16_t reserved_4;
	uint16_t iomap_base;
} PACKED tss_t;

typedef struct {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t  base_mid;
    uint8_t  flags0;
    uint8_t  flags1;
    uint8_t  base_hi;
} PACKED tss_entry_lo_t;

typedef struct {
    uint32_t base_highest;
    uint32_t reserved;
} PACKED tss_entry_hi_t;

typedef struct {
    uint16_t limit_lo;      // 0-15 
    uint16_t base_lo;       // 16-31 
    uint8_t base_mid;     // 32-39
    uint8_t access;          // 40-47
    uint8_t granularity;     // 48-55
    uint8_t base_hi;       // 56-63
} PACKED gdt_entry_t;

/* gdtr */
typedef struct {
	uint16_t limit;
	uintptr_t base;
} PACKED gdt_pointer_t;

typedef struct  {
	gdt_entry_t entries[5];
    tss_entry_lo_t tss_lo;
    tss_entry_hi_t tss_hi;
} PACKED __attribute__((aligned(0x10))) gdt_t;

void gdt_install(void);
void gdt_reload(gdt_pointer_t *);
void gdt_dump(void);

void tss_init(void);
void tss_install(void);



#endif /* ifndef GDT_H */
