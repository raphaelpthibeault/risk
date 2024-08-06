#include "macros.h"
#include <boot/protocol.h>
#include <mm/pmm/pmm.h>
#include <mm/paging/paging.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <boot/limine/limine.h>
#include <io/uart/uart.h>
#include <lib/std/printf.h>
#include <lib/std/memset.h>

#define LIMINE_REQUEST __attribute__((used, section(".requests")))


LIMINE_REQUEST
static volatile LIMINE_BASE_REVISION(2);

LIMINE_REQUEST
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

LIMINE_REQUEST
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

LIMINE_REQUEST
static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

LIMINE_REQUEST
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
};

LIMINE_REQUEST
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
};

LIMINE_REQUEST
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
};


/*
__attribute__((used, section(".limine_requests")))
static volatile struct limine_entry_point_request entry_point_request = {
    .id = LIMINE_ENTRY_POINT_REQUEST,
    .revision = 0,
    .response = NULL,
    .entry = &_some_assembly_function_perhaps?
};

*/

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Halt and catch fire
static 
inline 
ALWAYSINLINE 
void
hcf(void) {
    asm ("cli");
    for (;;) {
        __asm__ ("hlt");
    }
}
static
inline 
ALWAYSINLINE
void 
cli(void) {
    __asm__ ("cli");
}
static 
inline 
ALWAYSINLINE 
void 
sti(void) {
    __asm__ ("sti");
}



static  
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

static 
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

void 
limine_responses_checks(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        printf("[error] base revision not supported\r\n");
        hcf();
    }
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        printf("[error] framebuffer request\r\n");
        hcf();
    }
    if (smp_request.response == NULL) {
        printf("[error] smp request\r\n");
        hcf();
    }
    if (kernel_address_request.response == NULL) {
        printf("[error] kernel address request\r\n");
        hcf();
    }
    if (memmap_request.response == NULL) {
        printf("[error] memmap request\r\n");
        hcf();
    }
    if (module_request.response == NULL) {
        printf("[debug] module response is NULL; reason: there being 0 modules in limine config file. TODO: terminal should be a module.\r\n");
    }
    if (rsdp_request.response == NULL) {
        printf("[error] rsdp request\r\n");
        hcf();
    }
}

memory_map_t *memory_map;
framebuffer_info_t framebuffer;

static uint64_t _kernel_physical_load;
static uint64_t _kernel_virtual_load;

void * 
get_kernel_physical_load() {
    return (void *) _kernel_physical_load;
}

uint64_t
get_kernel_virtual_offset() {
    return _kernel_virtual_load - _kernel_physical_load;
}

void 
limine_responses_parse(void) {
    uint64_t limine_map_entries_nb;
    uint64_t map_pages;
    uint64_t i;
    struct limine_memmap_entry* used_entry = NULL;
    struct limine_memmap_entry* free_entry = NULL;


    // acpi: load rsdp address


    struct limine_memmap_entry **limine_map = memmap_request.response->entries;
    limine_map_entries_nb = memmap_request.response->entry_count;
    
    _kernel_physical_load = kernel_address_request.response->physical_base;
    _kernel_virtual_load = kernel_address_request.response->virtual_base;

    map_pages = (limine_map_entries_nb * sizeof(struct limine_memmap_entry)) / PAGING_PAGE_SIZE;  
    if (((limine_map_entries_nb * sizeof(struct limine_memmap_entry)) % PAGING_PAGE_SIZE) != 0)
        ++map_pages;

    /* TODO: increase map_pages for modules */


    /* modifying the limine memory map to carve out a region of memory to store the new memory map in and 
     * then protecting that memory from the PMM
     *
     * This is some black magic. 
     * I try to find 2 consecutive entries, the first being used, the second being free
     * i.e. 
     * [   used   |        free             ]
     * the idea being I need memory to store my new memory map (for paging and stuff)
     * so I put that new memory map in the free section and extend the used section (so the PMM knows the memory is being used)
     * [ old used | new mm |    free        ]
     * [       new used    |    free        ]
     * this avoids the hassle of trying to create new entries + edit current entries in the limine map
     * */

    for (i = 0; i < limine_map_entries_nb; ++i) {
        if ((limine_memtype_convert(limine_map[i]->type) == MEMORY_MAP_BUSY)
            && limine_map[i+1]->type == LIMINE_MEMMAP_USABLE
            && ((limine_map[i+1]->length / PAGING_PAGE_SIZE) > map_pages)
        ) {
            used_entry = (struct limine_memmap_entry *)limine_map[i];
            free_entry = (struct limine_memmap_entry *)limine_map[i+1];
            break;
        }
    }

    if (!used_entry || !free_entry) {
        printf("[error] could not find a [ used | free ] block\r\n");
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }

    void *candidate_base = (void *)free_entry->base;

    int size = map_pages * PAGING_PAGE_SIZE;
    used_entry->length += size;
    free_entry->base += size;
    free_entry->length -= size;
    
    /* make new memory map */
    memory_map = (memory_map_t *)free_entry->base;

    printf("[debug] candidate_base = %p\n", candidate_base);
    printf("[debug] Size of memory_map_t = %llu\n", sizeof(memory_map_t));
    printf("[debug] pointer location: %p\r\n", (memory_map_entry_t *)((uint64_t)candidate_base + sizeof(memory_map_t)));
    printf("[debug] memory_map: %p\n", memory_map);
    printf("[debug] memory_map->entries: %p\n", memory_map->entries);

    /*

[debug] candidate_base = 52000
[debug] Size of memory_map_t = 16
[debug] pointer location: 52010
[debug] memory_map: 53000

     * */





    memory_map->entries = (memory_map_entry_t *)((uint64_t)candidate_base + sizeof(memory_map_t));
    memory_map->entry_nb = limine_map_entries_nb;

    printf("[debug] memory_map created at %p with %llu entries\r\n", memory_map, memory_map->entry_nb);

    for (i = 0; i < memory_map->entry_nb; ++i) {
        memory_map->entries[i].base = limine_map[i]->base;
        memory_map->entries[i].len = limine_map[i]->length;
        memory_map->entries[i].sig = limine_memtype_convert(limine_map[i]->type);
    }

    /* make the first MB unusable to avoid conflicts with real mode
     * see https://wiki.osdev.org/Memory_Map_(x86)
     * */

    memory_map->entries[0].base = 0x0000000000000000;
    memory_map->entries[0].len = 0x0000000000100000;
    memory_map->entries[0].sig = MEMORY_MAP_NOUSE;

    for (i = 0; i < memory_map->entry_nb; ++i) {
        if (memory_map->entries[i].base < 0x0000000000100000) {
            if (memory_map->entries[i].base + memory_map->entries[i].len < 0x0000000000100000) {
                memory_map->entries[i].len = 0;
                memory_map->entries[i].sig = MEMORY_MAP_NOUSE;
            } else {
                memory_map->entries[i].len -= (0x0000000000100000 - memory_map->entries[i].base);
                memory_map->entries[i].base = 0x0000000000100000;
            }
        } else  {
            break;
        }
    }

    // assume the framebuffer model is RGB with 32-bit pixels.
    struct limine_framebuffer *fb_response = framebuffer_request.response->framebuffers[0];
    framebuffer.frame_addr = (uint64_t)fb_response->address;
    framebuffer.frame_bpp = fb_response->bpp;
    framebuffer.frame_pitch = fb_response->pitch;
    framebuffer.frame_width = fb_response->width;
    framebuffer.frame_height = fb_response->height;

    /* TODO: module stuff ; need write the terminal first */

}

void 
_start(void) {
    serial_shell_enable();
    printf("[debug] serial shell started!\r\n");
    limine_responses_checks();

    uint64_t i;

    uint64_t nb_cpus = smp_request.response->cpu_count;
    printf("[info] BSP id: %d\r\n", smp_request.response->bsp_lapic_id);
    printf("[info] Nb CPUs: %d\r\n", nb_cpus);

    for (i = 0; i < nb_cpus; i++) {
        struct limine_smp_info *cpu_info = smp_request.response->cpus[i];
        printf("Core %d: With ID: %d\r\n", i, cpu_info->lapic_id);
    }

    printf("[info] physical base: %llu\r\n", kernel_address_request.response->physical_base);
    printf("[info] virtual base: %llu\r\n", kernel_address_request.response->virtual_base);

    printf("[info] memory mapping BEGIN\r\n");
    struct limine_memmap_response *memmap_response = memmap_request.response;
    for (i = 0; i < memmap_response->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap_response->entries[i];
        printf("Entry %llu: base: %llu, type: %llu -> %s -> %s, length %llu\r\n",
               i, entry->base, entry->type, limine_memtype_to_str(entry->type), memtype_to_str(limine_memtype_convert(entry->type)), entry->length);
    }
    printf("[info] memory mapping END\r\n");
    printf("[info] no modules\r\n");
    printf("[info] RSDP get was gotten\r\n");


    /* recall only the BSP runs this _start function */
    cli();
    /* gdt */ 
    /* idt */ 
    // sti();

    /* limine_parse */
    /*
    printf("[debug] limine responses parse being called\r\n");
    limine_responses_parse();
    printf("[debug] limine responses parse DONE. Results: \r\n");
    */


    /* pmm init */
    /* paging reload kernel map */





    hcf();
}

