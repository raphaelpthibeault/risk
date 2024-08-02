#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <limine.h>
#include <serial.h>
#include <log.h>
#include <asm.h>

// Set the base revision to 2, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

// Framebuffer request
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// SMP request
__attribute__((used, section(".requests")))
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

// Kernel Address request.
__attribute__((used, section(".requests")))
static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

void  
bootloader_checks(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        log_("Error: Limine base revision unsupported\n");
        hcf();
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1 ||
        smp_request.response == NULL ||
        kernel_address_request.response == NULL) {
        log_("Error: Limine request\n"); 
        hcf();
    }
}

/* initialize the core kernel
 * only CPU 0 runs the _start function 
 */
void 
_start(void) {
    log_("Starting risk\n");
    bootloader_checks();
    log_("Bootloader checked\n");

    /* initialize COM1 */
    init_serial();

    /* disable interrupts */
    disable_interrupts();



    /* re-enable interrupts */
    enable_interrupts();

    /* some logging */
    uint64_t processor_id = smp_request.response->bsp_lapic_id;
    uint64_t kernel_start = kernel_address_request.response->virtual_base;

    log_("Main CPU: %d\n", processor_id);
    uint64_t nb_cpus = smp_request.response->cpu_count;
    log_("Number of cpus detected: %d\n", nb_cpus);
    
    if (kernel_start != 0xFFFFFFFF80000000) {
        // higher half kernel so kernel_start should be 0xFFFFFFFF80000000 = 18446744071562067968
        log_("Error: kernel start::= %llu != 0xFFFFFFFF80000000\n", kernel_start);
    }

    /* booting code for all cpus, put in a smp file/folder. Just do cpu 0 for now
     * 

    for (uint64_t i = 0; i < nb_cpus; i++) {
        struct limine_smp_info *cpu_info = smp_request.response->cpus[i];
        log_("Core %d: With ID: %d\n", i, cpu_info->lapic_id);

    }
    */

    /* some drawing */
    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    // draws a 0xffffff (white) line onto the screen
    for (size_t i = 0; i < 250; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    // for now just hang and freeze the emulator
    // should be a panic, eventually
    hcf();
}
