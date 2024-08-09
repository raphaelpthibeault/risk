#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <macros.h>
#include <boot/limine/limine.h>

#define MEMORY_MAP_FREE     0x00
#define MEMORY_MAP_BUSY     0x01
#define MEMORY_MAP_MMIO     0x02
#define MEMORY_MAP_NOUSE    0x03

#define MEMORY_AMD_IOMMU_BLOCK_START    0xFD00000000
#define MEMORY_AMD_IOMMU_BLOCK_END      0xFFFFFFFFFF


#define MEMORY_PAGE_SIZE		0x1000
#define	MEMORY_PAGE_ENTRY_SIZE	(MEMORY_PAGE_SIZE/sizeof(long))
#define MEMORY_MAP_MAX_ENTRIES	256
#define	ROUND_UP_PAGE_SIZE(n)	ROUND_UP(n, MEMORY_PAGE_SIZE)
#define	NEAREST_PAGE(n)			(ROUND_UP_PAGE_SIZE(n) / MEMORY_PAGE_SIZE)

#define MEMORY_IS_ADDR_PAGE_ALIGNED(ADDR) ((ADDR) % MEMORY_PAGE_SIZE == 0)
#define MEMORY_IS_SIZE_PAGE_ALIGNED(SIZE) ((SIZE) % MEMORY_PAGE_SIZE == 0)

#define MEMORY_IS_RANGE_PAGE_ALIGNED(RANGE)       \
    (MEMORY_IS_ADDR_PAGE_ALIGNED((RANGE).base) && \
     MEMORY_IS_SIZE_PAGE_ALIGNED((RANGE).length))


typedef struct {
    uint32_t    frame_width;
    uint32_t    frame_height;
    uint64_t    frame_addr;
    uint32_t    frame_pitch;
    uint32_t    frame_bpp;
} framebuffer_info_t;

framebuffer_info_t *get_framebuffer(void);

inline 
char *
limine_memtype_to_str(uint64_t t) {
    switch (t) {
        case LIMINE_MEMMAP_USABLE: return "Usable";
        case LIMINE_MEMMAP_RESERVED: return "Reserved";
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE: return "ACPI reclaimable";
        case LIMINE_MEMMAP_ACPI_NVS: return "ACPI NVS";
        case LIMINE_MEMMAP_BAD_MEMORY: return "Bad memory";
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: return "Bootloader reclaimable";
        case LIMINE_MEMMAP_KERNEL_AND_MODULES: return "Kernel and modules";
        case LIMINE_MEMMAP_FRAMEBUFFER: return "Framebuffer";
        default: return "ERROR: UNIDENTIFIABLE MEMTYPE";
    }
}

inline
uint64_t 
limine_memtype_convert(uint64_t t) {
    switch (t) {
        case LIMINE_MEMMAP_USABLE: return MEMORY_MAP_FREE;
        case LIMINE_MEMMAP_RESERVED: return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE: return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_ACPI_NVS: return MEMORY_MAP_NOUSE;
        case LIMINE_MEMMAP_BAD_MEMORY: return MEMORY_MAP_NOUSE;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_KERNEL_AND_MODULES: return MEMORY_MAP_BUSY;
        case LIMINE_MEMMAP_FRAMEBUFFER: return MEMORY_MAP_MMIO;
        default: return MEMORY_MAP_NOUSE;
    }
}

