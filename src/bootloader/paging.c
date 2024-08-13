#include <paging.h>
#include <stdint.h>

static void map_page(pml_t *pml, void *virtual_address, void *physical_address, uint16_t flags);
static void *alloc_page(void);
static pml_entry_t pml_entry_create(void* address, uint64_t flags);


pml_t *pml_create(void) {
    pml_t *pml = alloc_page();
    SetMem(pml, EFI_PAGE_SIZE, 0);
    return pml;
}

void 
map_pages(pml_t *pml, void* virtual_address, void *physical_address, uint64_t nb_pages, uint16_t flags) {
    for (uint64_t page = 0; page < nb_pages; page++) {
        map_page(pml, (void*)((uint64_t)virtual_address + page * EFI_PAGE_SIZE), (void*)((uint64_t)physical_address + page * EFI_PAGE_SIZE),
            flags);
    }
}

/* privates */

static 
void 
map_page(pml_t *pml, void *virtual_address, void *physical_address, uint16_t flags) {
    if ((uint64_t)virtual_address % EFI_PAGE_SIZE != 0) {
        Print(L"[error] map_page() : virtual address is not page aligned!\r\n");
        for ( ; ; ) 
            asm volatile("hlt");
    }
    if ((uint64_t)physical_address % EFI_PAGE_SIZE != 0) {
        Print(L"[error] map_page() : physical address is not page aligned!\r\n");
        for ( ; ; ) 
            asm volatile("hlt");
    }
}

static 
void *
alloc_page(void) {
    EFI_PHYSICAL_ADDRESS addr = 0;
    EFI_STATUS status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, 1, &addr);
    if (EFI_ERROR(status)) {
        Print(L"[error] alloc_page() BS->AllocatePages failed\r\n");
        for ( ; ; ) 
            asm volatile("hlt");
    }

    return (void*)addr;
}

static 
pml_entry_t
pml_entry_create(void *address, uint64_t flags) {
    return ((((uintptr_t)address >> 12) & 0x000000FFFFFFFFFF) << 12) | (flags | (uint64_t)PAGE_PRESENT);
}

