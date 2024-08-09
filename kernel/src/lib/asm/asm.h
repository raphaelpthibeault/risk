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

inline 
ALWAYSINLINE
void 
cli(void) {
    __asm__("cli");
}

static 
inline 
ALWAYSINLINE 
void 
sti(void) {
    __asm__ ("sti");
}

inline 
ALWAYSINLINE
void 
hlt(void) {
    __asm__ ("hlt");
}

