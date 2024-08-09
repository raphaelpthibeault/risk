#include <macros.h>
#include <boot/protocol.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <boot/limine/limine.h>
#include <io/uart/uart.h>
#include <lib/std/printf.h>
#include <lib/std/memset.h>
#include <mm/pmm/pmm.h>
#include <cpu/cpu.h>
#include <lib/asm/asm.h>
#include <cpu/gdt/gdt.h>

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

LIMINE_REQUEST
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

LIMINE_REQUEST 
static volatile struct limine_paging_mode_request paging_mode_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .mode = LIMINE_PAGING_MODE_X86_64_4LVL
};

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

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
    if (hhdm_request.response == NULL) {
        printf("[error] hhdm request\r\n");
        hcf();
    }
    if (paging_mode_request.response == NULL || paging_mode_request.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
        printf("[error] paging mode request\r\n");
        hcf();
    }
}

framebuffer_info_t _framebuffer;

void 
setup_framebuffer(void) {
    // assume the framebuffer model is RGB with 32-bit pixels.
    struct limine_framebuffer *fb_response = framebuffer_request.response->framebuffers[0];
    _framebuffer.frame_addr = (uint64_t)fb_response->address;
    _framebuffer.frame_bpp = fb_response->bpp;
    _framebuffer.frame_pitch = fb_response->pitch;
    _framebuffer.frame_width = fb_response->width;
    _framebuffer.frame_height = fb_response->height;
}


framebuffer_info_t *
get_framebuffer(void) {
    return &_framebuffer;
}


void 
_start(void) {
    /* actually need memory for the shell (input), so going to have to move the code to some serial_COM_init function
     * so I can keep the logging
     * */

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
        printf("Entry %llu: base: %llu, type: %llu -> %s, length %llu\r\n",
               i, entry->base, entry->type, limine_memtype_to_str(entry->type), entry->length);
    }
    printf("[info] memory mapping END\r\n");
    printf("[info] no modules\r\n");
    printf("[info] RSDP address %p\r\n", rsdp_request.response->address);
    uint64_t hhdm_start = hhdm_request.response->offset; // should be 0xFFFF800000000000
    printf("[info] HHDM start %llu\r\n", hhdm_start);
    if (hhdm_start != 0xFFFF800000000000) {
        printf("[error] earliest possible start location in a 4-level paging scheme is not 0xFFFF800000000000\r\n");
    }

    /* setup steps:
     * pre bsp:
     *  - terminal 
     *  - memory
     *  - debugging
     *  - acpi
     *  - interrupt preparation
     *
     * setup bsp 
     *
     * post bsp:
     *  - smp
     *  - tasking
     *
     * */
    setup_framebuffer();

    /* recall only the BSP runs this _start function */
    cpu_t *bsp = cpu_create(smp_request.response->bsp_lapic_id);
    printf("[info] BSP initialized\r\n");
    cpu_disable_interrupts();
    cpu_retain_disable();

    gdt_assemble();
    gdt_dump();
    


    pmm_init(memmap_response);

    cpu_retain_enable();
    cpu_enable_interrupts();


    hcf();
}

