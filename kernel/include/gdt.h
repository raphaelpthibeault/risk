#pragma once

#include <macros.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

/*
 * GDT is specific to the IA-32 and x86-64 architectures
 *
 * triple fault should cause shutdown, so I don't think I need to cover for that, also there's no error vector.
 * But I do want to cover all the other faults 
 * */ 

#define GDT_NUM		8	// 2 null segments + 2 code + 2 data segments + TSS descriptor is the size of 2 GDT entries
#define GDT_TSS_NUM	1	// only 1 TSS, TODO: add a double fault TSS

/* SEGMENT REGISTER (GDTR) OFFSET */
#define OFFSET_KERNEL_CODE_SEG      0x08
#define OFFSET_KERNEL_DATA_SEG      0x10
#define OFFSET_USER_CODE_SEG        0x18 
#define OFFSET_USER_DATA_SEG        0x20
#define OFFSET_TSS_SEG              0x28

/* GDT ENTRY */
typedef struct s_gdt_entry {
    uint16_t limit_lo;      /* limit 0-15 */
    uint16_t base_lo;       /* base 0-15 */
    uint8_t base_mid;       /* base 16-23 */
    uint8_t access;         /* present(1), ring(2), type(1), executable(1), conforming(1), writable(1), accessed(1) */
    uint8_t flags;          /* granularity(1), protected-mode(1), long-mode(1), available(1), limit_high(4) */
    uint8_t base_hi;        /* base 24-31 */
} PACKED gdt_entry_t;

/* The GDT is pointed to by the value in the GDTR register.
 * This is loaded using the LGDT assembly instruction (gdt.asm), whose argument is a pointer to a GDT Descriptor structure (this definition)
 */
typedef struct s_gdt_descriptor {
    uint16_t size;          /* the size of the table in bytes subtracted by 1. This subtraction occurs because the maximum value of Size is 65535, while the GDT can be up to 65536 bytes in length (8192 entries). Further, no GDT can have a size of 0 bytes */
    gdt_entry_t *offset;    /* the linear address of the GDT (not the physical address, paging applies) */
} PACKED gdt_descriptor_t;

enum gdt_access {
    GDT_ACCESS_WRITABLE		= 0b00000010,
	GDT_ACCESS_TYPE			= 0b00010000,
	GDT_ACCESS_PRESENT		= 0b10000000,
	GDT_ACCESS_EXECUTABLE	= 0b00001000,
	GDT_ACCESS_DPL			= 0b01100000,	// segment privilege level is ring 3
};

enum gdt_flags {
	GDT_FLAG_LONG_MODE			= 0b00100000,
	GDT_FLAG_PAGE_GRANULARITY	= 0b10000000,
};

typedef struct s_tss_entry {
	uint32_t	reserved0;
	uint32_t	rsp[3 * 2];		// ring stacks
	uint32_t	reserved1[2];
	uint32_t	ist[7 * 2];		// interrupt stack tables
	uint32_t	reserved2[2];
	uint16_t	reserved3;
	uint16_t	io_map_base;
} PACKED tss_entry_t;

typedef struct s_tss_descriptor {
    uint16_t	limit_lo;	// Segment limit 0-15
	uint16_t	base_lo;	// Base 0-15
	uint8_t		base_mid;	// Base 16-23
	uint8_t		access;		// present(1), ring(2), reserved(1), type(4)
	uint8_t		flags;		// granularity(1), reserved(2), available(1), limit_high(4)
	uint8_t		base_mid2;	// 24-31
	uint32_t	base_hi;	// 32-63
	uint32_t	reserved;
} PACKED tss_descriptor_t;

/* struct to hold the gdt at runtime */
typedef struct s_gdt_block {
    gdt_descriptor_t gdtd;
    gdt_entry_t gdt[GDT_NUM];
    tss_entry_t tss[GDT_TSS_NUM];
} gdt_block_t;


/* initialize the gdt descriptor with 
 * */
void gdt_init(gdt_block_t *);

/* sets the gdt_block at index with an access and a flag
 * */
void set_gdt_entry(gdt_block_t *, size_t, enum gdt_access, enum gdt_flags);

/* set up a new task state segment at specified index
 */
void set_gdt_tss(gdt_block_t *, size_t);

/* load the gdt descriped by the gdt_descriptor
 * */
void load_gdt(gdt_descriptor_t *);

