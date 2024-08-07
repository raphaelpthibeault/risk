#include "apic.h"
#include <stdint.h>

uint32_t _lapic_base = 0;


uint32_t 
lapic_current_cpu(void) {
    /* TODO */
    return 0;
    /*
    if (_lapic_base == 0) {
        return 0;
    } else {
        // get cpu # from apic register        
    }*/
}

