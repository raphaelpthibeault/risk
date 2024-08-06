#pragma once 

#include <macros.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 4-level paging described in the Intel Manual Chapter 4
 */

#define PAGING_PAGE_SIZE            0x1000
#define PAGING_PAGE_ALIGNED __attribute__((aligned(PAGING_PAGE_SIZE)))

typedef uint64_t paging_desc_t; /* page table entries */

/* a struct to hold the table entry locations extracted from a linear address */
typedef struct {
    uint16_t    pml4; /* PML4 Table */
    uint16_t    pml3; /* Page-Directory-Pointer Table (PDPT) */
    uint16_t    pml2; /* Page Directory Table (PDT) */
    uint16_t    pml1; /* Page Table (PT) */
} paging_indexer_t;

typedef struct { 
    paging_desc_t entries[512];
} PAGING_PAGE_ALIGNED paging_table_t;

static 
ALWAYSINLINE
inline 
void 
paging_desc_set_addr(paging_desc_t *descriptor, uint64_t addr) {
    *descriptor |= (uint64_t)addr & 0xffffffffff000; // mask out lower 12 bits to maintain alignment
}

/* extract table entry locations from a 48-bit linear address */
static 
ALWAYSINLINE
inline
void 
paging_indexer_assign(paging_indexer_t *indexer, void *linear_address) {
    uint64_t p = (uint64_t)linear_address;
    p >>= 12; 
    indexer->pml1 = p & 0x1ff;  /* 12-20 */ 
    p >>= 9;
    indexer->pml2 = p & 0x1ff;  /* 21-29 */
    p >>= 9;
    indexer->pml3 = p & 0x1ff;  /* 30-38 */
    p >>= 9;
    indexer->pml4 = p & 0x1ff;  /* 39-47 */
}

static
ALWAYSINLINE
inline
void *
paging_desc_get_addr(paging_desc_t *descriptor) {
    return (void*)(*descriptor & 0xffffffffff000); // mask out lower 12 bits to maintain alignment
}

static
ALWAYSINLINE
inline
bool 
paging_desc_get_flag(paging_desc_t *descriptor, uint64_t flag) {
    return *descriptor & flag;
}

static
ALWAYSINLINE
inline
void 
paging_desc_set_flag(paging_desc_t *descriptor, uint64_t flag, bool value) {
    *descriptor &= ~flag;
    if (value)
        *descriptor |= flag;
}

static
ALWAYSINLINE
inline
void 
paging_desc_set_flags(paging_desc_t *descriptor, uint64_t flags) {
    *descriptor &= ~0x0ffful;
    *descriptor |= flags;
}

static
ALWAYSINLINE
inline
void 
paging_invlpg(void* page) {
    __asm__ volatile ("invlpg (%0)" : : "r"(page) : "memory");
}

void *paging_map_page(void* virt, void* phys, uint16_t flags);
void  paging_unmap_page(void* virt);
void *paging_remap_page(void* old_virt, void* new_virt);
void *paging_walk_page(void* virt);
void *paging_edit_page(void* virt, uint16_t flags);
void  paging_load_pml4(paging_table_t* pml4);
void *paging_get_pml4();

