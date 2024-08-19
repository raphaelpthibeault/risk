#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <macros.h>
#include <stdint.h>

typedef struct {
    uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;

	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;

	uint32_t mmap_length;
	uint32_t mmap_addr;

	uint32_t drives_length;
	uint32_t drives_addr;

	uint32_t config_table;

	uint32_t boot_loader_name;

	uint32_t apm_table;

	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;

	uint64_t framebuffer_addr;
	uint32_t framebuffer_pitch;
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	uint8_t  framebuffer_bpp;
	uint8_t  framebuffer_type;
} PACKED multiboot_t;

typedef struct {
	uint32_t type;
	uint32_t length;
} multiboot2_tag_header_t;

typedef struct {
	multiboot2_tag_header_t head;
	uint32_t entry_length;
	uint32_t entry_version;
	char entries[];
} multiboot2_memmap_t;

typedef struct {
    uintptr_t highest_valid_address;
    uintptr_t highest_kernel_address;
} multiboot2_addresses_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} multiboot2_memmap_entry_t;

typedef struct {
    multiboot2_tag_header_t head;
	uint32_t module_start;
	uint32_t module_end;
	uint8_t cmdline[];
} multiboot2_module_t;

multiboot2_addresses_t multiboot2_init(multiboot_t *);

#endif /* ifndef MULTIBOOT_H */
