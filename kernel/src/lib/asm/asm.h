#pragma once 
#include <macros.h>

/* halt and catch fire 
 */
inline 
ALWAYSINLINE 
void
hcf(void) {
    #ifdef __x86_64__
    __asm__ ("cli");
    for (;;) {
        __asm__ ("hlt");
    }
    #endif
}

