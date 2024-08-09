#include "heap.h"
#include <mm/pmm/pmm.h>
#include <boot/protocol.h>
#include <lib/sync/lock.h>
#include <macros.h>
#include <mm/mmap.h>
#include <stdint.h>
#include <lib/eh/eh.h>

static lock_t _lock;

/* I've got it so that for a CPU to allocate or free from the heap they must acquire the heap lock, AND the pmm lock 
 * (because the pmm has its own lock)
 * hopefully this should avoid any undefined shenanigans
 * */

static 
HEAP_RESULT allocate(uint64_t length, bool upper) {
    lock_acquire(&_lock);

    length = ALIGN_UP(length, MEMORY_PAGE_SIZE);
    memory_range_t range = TRY(HEAP_RESULT, pmm_alloc(length, upper)); // physical range
    range.base = map_paddr_to_vaddr(range.base); // virtual range

    lock_release(&_lock);

    return OK(HEAP_RESULT, range);
}

HEAP_RESULT 
heap_alloc(uint64_t length) {
    return allocate(length, true);
}

HEAP_RESULT 
heap_alloc_lower(uint64_t length) {
    return allocate(length, false);
}

HEAP_RESULT 
heap_free(memory_range_t range) {
    lock_acquire(&_lock);

    range.base = map_vaddr_to_paddr(range.base);
    TRY(HEAP_RESULT, pmm_unused(range));

    lock_release(&_lock);
    
    return OK(HEAP_RESULT, range);
}

memory_range_t 
map_heap_range_to_pm_range(memory_range_t range) {
    range.base = map_vaddr_to_paddr(range.base);
    return range;
}

