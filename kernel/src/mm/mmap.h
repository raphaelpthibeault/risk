#pragma once 
#include <stdint.h>

#ifdef __x86_64__
#define VIRTUAL_BASE 0xffff800000000000
#define KERNEL_BASE 0xffffffff80000000
#endif

static
inline
uint64_t 
map_paddr_to_vaddr(uint64_t paddr) {
    return paddr + VIRTUAL_BASE;
}

static 
inline
uint64_t 
map_vaddr_to_paddr(uintptr_t vaddr) {
    return vaddr - VIRTUAL_BASE;
}

static 
inline 
uint64_t
map_paddr_to_kaddr(uintptr_t paddr) {
    return paddr + KERNEL_BASE;
}

