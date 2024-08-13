#ifndef BOOTLOADER_PAGING_H

#include <bootloader/protocol.h>
#include <stdint.h>

/* temporary bootloader paging
 * */

#define PML_ENTRY_COUNT 512
#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE (1 << 1)
#define PAGE_USER (1 << 2)
#define PAGE_WRITE_TROUGH (1 << 3)
#define PAGE_CACHE_DISABLED (1 << 4)
#define PAGE_ACCESSED (1 << 5)
#define PAGE_DIRTY (1 << 6)
#define PAGE_PAGE_SIZE (1 << 7)
#define PAGE_GLOBAL (1 << 8)

#define PML_GET_ADDRESS(entry) ((entry) & 0x000ffffffffff000)
#define PML_LOAD(pageTable) asm volatile("mov %0, %%cr3" : : "r"((uint64_t)pageTable))

typedef uint64_t pml_entry_t;

typedef struct {
    pml_entry_t entries[PML_ENTRY_COUNT];
} pml_t;


pml_t *pml_create(void);

void map_pages(pml_t * pml, void* virtual_address, void *physical_address, uint64_t nb_pages, uint16_t flags);


#endif /* ifndef BOOTLOADER_PAGING_H */
