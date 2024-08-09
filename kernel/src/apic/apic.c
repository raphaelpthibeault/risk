#include "apic.h"
#include <stdint.h>
#include <lib/mem/volatile.h>

uint32_t _lapic_base = 0;

uint32_t 
lapic_current_cpu(void) {
    if (_lapic_base == 0) {
        return 0;
    } else {
        return lapic_read(LAPIC_CPU_ID) >> 24;
    }
}

uint32_t  
lapic_read(uint32_t reg) {
    return volatile_read32((uintptr_t)_lapic_base + reg);
}

void 
lapic_write(uint32_t reg, uint32_t val) {
    volatile_write32((uintptr_t)_lapic_base + reg, val);      
}

void 
lapic_eoi(void) {
    lapic_write(LAPIC_REG_EOI, 0);
}

