#include "sync.h"
#include <cpu/cpu.h>

void
sync_enter(void) {
   cpu_retain_interrupts();
}

void 
sync_leave(void) {
   cpu_release_interrupts();
}

void 
sync_pause(void) {
   __asm__ volatile ("pause");
}

