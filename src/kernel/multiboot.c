#include <multiboot.h>
#include <stdint.h>
#include <stdio.h>

static 
void *
multiboot2_find_next(char *curr, uint32_t type) {
    char *header = curr;
    multiboot2_tag_header_t *tag;

    do {
        while ((uintptr_t)header & 7) ++header;
        tag = (void*)header;

        if (tag->type == 0) return NULL;
        if (tag->type == type) return tag;

        // get next tag
        header += tag->length;
    } while (1);
}

static 
void *
multiboot2_find_tag(void *mboot, uint32_t type) {
    char *header = mboot;
    header += 8;
    return multiboot2_find_next(header, type);
}

extern char end[];
uintptr_t highest_valid_address = 0;
uintptr_t highest_kernel_address = (uintptr_t)&end;

multiboot2_addresses_t
multiboot2_init(multiboot_t *mboot) {
    printf("[info] multiboot2 init\r\n");
    
    // memmap

    multiboot2_memmap_t *mmap = multiboot2_find_tag(mboot, 6);
    if (!mmap) {
        printf("[fatal] could not parse memory map from loader\r\n");
        while (1);
    }

    char *entry = mmap->entries;
    while ((uintptr_t)entry < (uintptr_t)mmap + mmap->head.length) {
        multiboot2_memmap_entry_t *curr = (void*)entry;

        if (curr->type == 1 && curr->length && (curr->base + curr->length - 1 > highest_valid_address))
            highest_valid_address = curr->base + curr->length - 1;

        entry += mmap->entry_length;
    }

    // modules
    
    multiboot2_module_t *mod = multiboot2_find_tag(mboot, 3);
    while (mod) {
        uintptr_t addr = (uintptr_t)mod->module_end;

        if (addr > highest_kernel_address) 
            highest_kernel_address = addr;

        mod = multiboot2_find_tag((char*)mod + mod->head.length, 3);
    }

    // align up a page
    highest_kernel_address = (highest_kernel_address + 0xfff) & 0xfffffffffffff000;

    return (multiboot2_addresses_t) {
        .highest_valid_address = highest_valid_address,
        .highest_kernel_address = highest_kernel_address,
    };
}

