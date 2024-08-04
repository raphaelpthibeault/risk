#pragma once

#include <macros.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

/* used for general kernel memory information handed over by the bootloader 
 * function implementations will be in pageframe allocator 
 * */

#define MEMORY_PAGE_SIZE                4096 /* 4-KByte pages (2**12) [may migrate to 2-MByte later ]*/
#define MEMORY_PAGE_ENTRY_SIZE          (MEMORY_PAGE_SIZE/sizeof(long)) 

#define ROUND_UP_PAGE_SIZE(n)           ROUND_UP(n, MEMORY_PAGE_SIZE)
#define NEAREST_PAGE(n)                 (ROUND_UP_PAGE_SIZE(n) / MEMORY_PAGE_SIZE)

#define MEMORY_MEMORYMAP_MAX_ENTRIES    256

enum memory_type {
    MEMORY_TYPE_USABLE, MEMORY_TYPE_UNUSABLE
};

typedef struct {
    void *physical_base;
    size_t num_pages;
    enum memory_type type;    
} PACKED memory_map_entry_t;

typedef struct {
    memory_map_entry_t entries[MEMORY_MEMORYMAP_MAX_ENTRIES];
    size_t num_entries;
} memory_map_t;


/* in pageframe allocator */
size_t memory_get_size_total();
size_t memory_get_size_used();
size_t memory_get_size_reserved();
size_t memory_get_free();

