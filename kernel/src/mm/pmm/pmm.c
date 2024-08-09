#include "pmm.h"
#include "lib/eh/eh.h"

#include <macros.h>
#include <boot/protocol.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <io/uart/uart.h>
#include <lib/std/printf.h>
#include <lib/std/memset.h>
#include <lib/sync/lock.h>
#include <lib/asm/asm.h>
#include <mm/mmap.h>

static bitmap_t _bitmap = {};
static memory_range_t _usable_range = {};
static uint64_t _used_memory = 0;
static uint64_t _available_memory = 0;
static lock_t _lock = {};
static uint64_t _upper_best_guess = 0;
static uint64_t _lower_best_guess = (uint64_t) - 1;

static bool range_collision(uint64_t,uint64_t,uint64_t,uint64_t);
static void bitmap_init(bitmap_t*,void*,uint64_t);
static void bitmap_fill(bitmap_t*,bool);
static uint64_t bitmap_set_range(bitmap_t*,memory_range_t,bool);
static bool bitmap_get(bitmap_t const *,uint64_t);
static void bitmap_set(bitmap_t*,uint64_t,bool);
static memory_range_t bitmap_find_free(bitmap_t const *,uint64_t,uint64_t,bool); 
static inline uint64_t bitmap_length(uint64_t);
static memory_range_t addr_to_pages(memory_range_t);
static memory_range_t pages_to_addr(memory_range_t);

/* initialize physical memory management with the bootloader's handover memory map 
 * note: the bootloader has already made the memory switch to virtual memory
 *       so no need to do anything with pageframes, just virtual pages
 * */
void 
pmm_init(struct limine_memmap_response const *memmap) {
    pmm_bitmap_init(memmap);
    pmm_mmap_load(memmap);

    // now declare the bitmap used!
    pmm_used((memory_range_t){map_vaddr_to_paddr((uintptr_t)_bitmap.data), _bitmap.length});
    printf("[info] pmm initialized!\r\n");
}

void 
pmm_bitmap_init(struct limine_memmap_response const *memmap) {
    printf("[info] allocating memory bitmap\r\n"); 
    uint64_t bitmap_len, i; 
    uint64_t start = memmap->entries[0]->base;
    uint64_t end = memmap->entries[memmap->entry_count-1]->base + memmap->entries[memmap->entry_count-1]->length;
    memory_range_t _usable_range = {.base = start, .length = end - start};
    printf("[info] usable range: [%llu, %llu]\r\n", start, end);

    bitmap_len = ALIGN_UP(_usable_range.length / MEMORY_PAGE_SIZE / BITMAP_SCALE, MEMORY_PAGE_SIZE);
    printf("[info] a bitmap of %llu b long is required.\r\n", bitmap_len);

    for (i = 0; i < memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length > bitmap_len) {
            // skip first 4 pages 
            if (range_collision(entry->base, entry->base + entry->length, 0, 4 * MEMORY_PAGE_SIZE)) 
                continue;
            
            bitmap_init(&_bitmap, (void*)map_paddr_to_vaddr(entry->base), bitmap_len);

            printf("[info] initialized bitmap at Entry %llu at %llu - %llu\r\n", i, entry->base, entry->base + bitmap_len - 1);
            break;
        }
    }

    /* specify the bits have been used */ 
    bitmap_fill(&_bitmap, PMM_USED);
    printf("[info] filled the bitmap successfully!\r\n");
}

void 
pmm_mmap_load(struct limine_memmap_response const* memmap) {
    printf("[info] loading the memmap\r\n");
    uint64_t i;

    for (i = 0; i < memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            memory_range_t range = {entry->base, entry->length};
            _used_memory += entry->length;
            pmm_unused(range);
            _available_memory += range.length;
        } else if (limine_memtype_convert(entry->type) == MEMORY_MAP_BUSY) {
            _available_memory += entry->length;
            _used_memory += entry->length;
        }
    }

    printf("[info] Available memory: %llu \r\n", _available_memory);
    printf("[info] Used memory: %llu\r\n", _used_memory);
    printf("[info] memmap loaded!\r\n");
}

PMM_RESULT 
pmm_unused(memory_range_t range) {
    lock_acquire(&_lock);
    
    printf("[info] pmm_unused(%llu, %llu)\r\n", range.base, range.length);
    if (range.base == 0) {
        printf("[error] Illegal address! Cannot unuse 0!\r\n");
        hcf();
    }

    bool is_mem_aligned = MEMORY_IS_RANGE_PAGE_ALIGNED(range);
    printf("[info] is_mem_aligned: %d\r\n", is_mem_aligned);
    if (!is_mem_aligned) {
        printf("[error] pmm_unused() memory is not page aligned!\r\n");
        hcf();
    }

    memory_range_t page_range = addr_to_pages(range);
    uint64_t num_pages = bitmap_set_range(&_bitmap, page_range, PMM_UNUSED);
    _used_memory -= num_pages * MEMORY_PAGE_SIZE;

    if (_upper_best_guess < (range.base + range.length))
        _upper_best_guess = range.base + range.length - 1;

    if (_lower_best_guess > range.base)
        _lower_best_guess = range.base;

    lock_release(&_lock);
    return OK(PMM_RESULT, range);
}

PMM_RESULT 
pmm_used(memory_range_t range) {
    lock_acquire(&_lock); 

    printf("[info] pmm_used(%llu, %llu)\r\n", range.base, range.length);
    
    bool is_mem_aligned = MEMORY_IS_RANGE_PAGE_ALIGNED(range);
    printf("[info] is_mem_aligned: %d\r\n", is_mem_aligned);
    if (!is_mem_aligned) {
        printf("[error] pmm_used() memory is not page aligned!\r\n");
        hcf();
    }

    memory_range_t page_range = addr_to_pages(range);
    uint64_t num_pages = bitmap_set_range(&_bitmap, page_range, PMM_USED);
    _used_memory += num_pages * MEMORY_PAGE_SIZE;

    lock_release(&_lock);
    return OK(PMM_RESULT, range);
}

PMM_RESULT 
pmm_alloc(uint64_t length, bool upper) {
    lock_acquire(&_lock); 
    printf("[info] pmm_alloc(%llu, %d)\r\n", length, upper);
    
    bool is_mem_aligned = MEMORY_IS_SIZE_PAGE_ALIGNED(length);
    printf("[info] is_mem_aligned: %d\r\n", is_mem_aligned);
    if (!is_mem_aligned) {
        printf("[error] pmm_alloc() memory is not page aligned!\r\n");
        hcf();
    }

    uint64_t page_count = length / MEMORY_PAGE_SIZE;
    /* use the best guess variables as a way to traverse memory, once we reach the end of memory we reset the guess variable(s) 
     * and start traversing memory from the start again
     * */
    memory_range_t page_range = bitmap_find_free(&_bitmap, upper ? _upper_best_guess : _lower_best_guess, page_count, upper);

    if (page_range.length == 0) { // couldn't find any free memory (starting from best guess
        if (upper) {
            _upper_best_guess = -1;
        } else {
            _lower_best_guess = 0;
        }
        // try again from the start
        page_range = bitmap_find_free(&_bitmap, upper ? _upper_best_guess : _lower_best_guess, page_count, upper);
    }

    if (page_range.length <= 0) {
        printf("[error] pmm_alloc() out of memory!\r\n");
        return ERR(PMM_RESULT, RISK_OUT_OF_MEMORY);
    }

    if (upper) {
        _upper_best_guess = page_range.base;
    } else {
        _lower_best_guess = page_range.base + page_range.length;
    }

    bitmap_set_range(&_bitmap, page_range, PMM_USED);

    memory_range_t pmm_range = pages_to_addr(page_range);
    _used_memory += pmm_range.length;

    lock_release(&_lock);
    return OK(PMM_RESULT, pmm_range);
}

/************************************************ privates ************************************************/

static 
bool
range_collision(uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
   return a < d && b > c; 
}

static
void 
bitmap_init(bitmap_t *bmap, void *data, uint64_t length) {
    *bmap = (bitmap_t){
        .data = data, 
        .length = length
    };
}

static
void 
bitmap_fill(bitmap_t *bmap, bool value) {
    if (value) {
        memset(bmap->data, 0xff, bmap->length);
    } else {
        memset(bmap->data, 0, bmap->length);
    }
}

static 
uint64_t 
bitmap_set_range(bitmap_t *bmap, memory_range_t range, bool value) {
    /* a rudimentary check to see if the bmap can handle the entry range
     * it is by no means a good (complete) check since there are many entries with different lengths that would need to be summed 
     * but this works for now */
    if ((range.base + range.length) > (bmap->length * BITMAP_SCALE)) {
        printf("[error] (page_range.base + page_range.length) > bmap->length\r\n");
        printf("page_range.base: %llu, page_range.length: %llu, sum: %llu; bmap->length * BITMAP_SCALE: %llu\r\n", 
               range.base, range.length, range.base + range.length, bmap->length * BITMAP_SCALE);
        hcf();
    }

    uint64_t num = 0, i;

    for (i = 0; i < range.length; ++i) {
        if (bitmap_get(bmap, range.base + i) != value) {
            bitmap_set(bmap, range.base + i, value);
            ++num;
        }
    }

    return num;
}

static
bool 
bitmap_get(bitmap_t const *bmap, uint64_t i) {
    uint64_t const byte_index = i / BITMAP_SCALE;
    uint64_t const bit_index = i % BITMAP_SCALE;
    return bmap->data[byte_index] & (1 << bit_index);
}

static
void 
bitmap_set(bitmap_t *bmap, uint64_t i, bool val) {
    uint64_t const byte_index = i / BITMAP_SCALE;
    uint64_t const bit_index = i % BITMAP_SCALE;
    if (val) {
        bmap->data[byte_index] |= (1 << (bit_index));
    } else {
        bmap->data[byte_index] &= ~(1 << (bit_index));
    }
}

static 
memory_range_t 
bitmap_find_free(bitmap_t const *bmap, uint64_t start, uint64_t length, bool upper) {
    uint64_t bmaplen = bitmap_length(bmap->length);

    if (bmaplen < 1) {
        printf("[error] bitmap length of < 1 in bitmap_find_free()\r\n");
        hcf();
    }

    if (bmaplen < start) {
        start = bmaplen; 
    }

    uint64_t range_start = 0;
    uint64_t range_length = 0;
    uint64_t i;

    for (i = start; upper ? i > 0 : i < bmaplen; upper ? --i : ++i) {
        if (bitmap_get(bmap, i) == 0) {
            if (range_length == 0 || upper) {
                range_start = i;
            }
            ++range_length;
        } else {
            range_start = 0;
            range_length = 0;
        }

        if (length == range_length) {
            return (memory_range_t){range_start, range_length};
        }
    }

    return (memory_range_t){};
}

static 
inline
uint64_t 
bitmap_length(uint64_t length) {
    return length * BITMAP_SCALE;
}

static 
memory_range_t 
addr_to_pages(memory_range_t range) {
    range.base -= _usable_range.base;
    range = range_div(range, MEMORY_PAGE_SIZE);
    return range;
}

static 
memory_range_t 
pages_to_addr(memory_range_t range) {
    range = range_mul(range, MEMORY_PAGE_SIZE);
    range.base += _usable_range.base;
    return range;
}

