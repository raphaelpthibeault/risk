#include <memory/mmu.h>
#include <memory/pml.h>

#define PAGE_SIZE      0x1000UL // 4 KB pages

#define _pagemap __attribute__((aligned(PAGE_SIZE))) = {0}
pml_t page_region_init[3][512] _pagemap;

