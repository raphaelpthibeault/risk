#pragma once 
#include <stdint.h>

static
inline 
uint32_t 
volatile_read32(uintptr_t addr) {
    return *((volatile uint32_t *)(addr));
}

static 
inline 
void 
volatile_write32(uintptr_t addr, uint32_t val) {
    *((volatile uint32_t *)(addr)) = val;
}

