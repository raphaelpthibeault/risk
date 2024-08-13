#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <efi.h>
#include <efilib.h>
#include <stdint.h>
#include <sys/list.h>
#include <sys/io.h>

#define EFI_RESERVED 0
#define EFI_LOADER_CODE 1
#define EFI_LOADER_DATA 2
#define EFI_BOOT_SERVICES_CODE 3
#define EFI_BOOT_SERVICES_DATA 4
#define EFI_RUNTIME_SERVICES_CODE 5
#define EFI_RUNTIME_SERVICES_DATA 6
#define EFI_CONVENTIONAL_MEMORY 7
#define EFI_UNUSABLE_MEMORY 8
#define EFI_ACPI_RECLAIM_MEMORY 9
#define EFI_ACPI_MEMORY_NVS 10
#define EFI_MEMORY_MAPPED_IO 11
#define EFI_MEMORY_MAPPED_IO_PORT_SPACE 12
#define EFI_PAL_CODE 13
#define EFI_PERSISTENT_MEMORY 14

typedef EFI_MEMORY_DESCRIPTOR efi_mem_desc_t;
/* 
 typedef struct {
    UINT32                          Type;           // Field size is 32 bits followed by 32 bit pad
    UINT32                          Pad;
    EFI_PHYSICAL_ADDRESS            PhysicalStart;  // Field size is 64 bits
    EFI_VIRTUAL_ADDRESS             VirtualStart;   // Field size is 64 bits
    UINT64                          NumberOfPages;  // Field size is 64 bits
    UINT64                          Attribute;      // Field size is 64 bits
} EFI_MEMORY_DESCRIPTOR;
 */

#define EFI_IS_MEMORY_AVAILABLE(type) \
    ((type == EFI_CONVENTIONAL_MEMORY) || (type == EFI_PERSISTENT_MEMORY) || (type == EFI_LOADER_CODE) || \
        (type == EFI_BOOT_SERVICES_CODE) || (type == EFI_BOOT_SERVICES_DATA))

#define EFI_MEMORY_MAP_GET_DESCRIPTOR(memory_map, index) \
    (efi_mem_desc_t*)((uint64_t)(memory_map)->base + ((index) * (memory_map)->descriptor_size))

typedef struct {
    efi_mem_desc_t *base;  
    uint64_t descriptor_count;
    uint64_t key;
    uint64_t descriptor_length;
    uint32_t descriptor_version;
} efi_mem_map_t;

/* graphics output protocol */ 
typedef struct {
    uint32_t *base;
    uint64_t size;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} gop_buffer_t;

typedef struct {
    list_entry_t *entry;
    char name[MAX_NAME];
    void *data;
    uint64_t length;
} ram_file_t;

typedef struct boot_info boot_info_t;

typedef struct {
    list_entry_t entry;
    char name[MAX_NAME];
    list_t children;
    list_t files;
} ram_dir_t;

typedef struct {
    void *physical_start;
    void *virtual_start;
    void (*entry)(boot_info_t*);
    uint64_t length;
} boot_kernel_t;

typedef struct boot_info {
    efi_mem_map_t memory_map;
    gop_buffer_t gop_buffer;
    ram_dir_t* ram_root;
    void* rsdp;
    void* runtime_services;
    boot_kernel_t kernel;
} boot_info_t;

#endif /* ifndef PROTOCOL_H */
