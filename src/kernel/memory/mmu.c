#include <memory/mmu.h>
#include <memory/pml.h>

#define PAGE_SIZE      0x1000UL // 4 KB pages

#define _pagemap __attribute__((aligned(PAGE_SIZE))) = {0}
pml_t page_region_init[3][512] _pagemap;

/* switch from loader + boot mappings to kernel mappings.
 * inits bitmap, high ident mapping, kernel heap, and asserts
 * all new kernel-space mappings apply to all kernel threads
 * */
void 
mmu_init(uint64_t memsize, uintptr_t first_free_page) {
    
}

/* frame address to virtual address
 * */
void *
mmu_map_from_physical(uintptr_t frame_addr) {
    return (void*)(frame_addr | HIGH_MAP_REGION);
}

