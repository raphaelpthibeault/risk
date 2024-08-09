#include "cpu.h"
#include <lib/asm/asm.h>
#include <stddef.h>
#include <macros.h>
#include <apic/apic.h>
#include <lib/std/printf.h>

static cpu_t _cpus[MAX_NUM_CPUS] = {};

cpu_t *
cpu_create(uint32_t id) {
    if (id > MAX_NUM_CPUS) 
        return NULL;

    cpu_t cpu;
    cpu.id = id;
    cpu.active = true;
    _cpus[id] = cpu;

    return &_cpus[id];
}

void 
cpu_initialize(cpu_t * cpu) {  
    /* TODO */
    // gdt's tss
}

cpu_t * 
cpu_get_current(void) {
    return &_cpus[lapic_current_cpu()];
}

void 
cpu_enable_interrupts(void) {
    sti(); 
}

void 
cpu_disable_interrupts(void) {
    cli();
}

void 
cpu_retain_disable() {
    cpu_get_current()->retain_enabled = false;
}

void
cpu_retain_enable() {
    cpu_get_current()->retain_enabled = true;
}

void 
cpu_retain_interrupts() {
    cpu_t *curr = cpu_get_current();

    if (curr->retain_enabled) {
        cpu_disable_interrupts();
        ++curr->retain_depth;
    }
}

void 
cpu_release_interrupts() {
    cpu_t *curr = cpu_get_current();

    if (curr->retain_enabled) {
        --curr->retain_depth;
        if (curr->retain_depth == 0) {
            cpu_enable_interrupts();
        }
    }
}

void 
cpu_being_interrupt() {
    cpu_t *curr = cpu_get_current();

    if (!curr->retain_enabled) {
        printf("[error] retain disabled when beginning an interrupt!\r\n");
        hcf();
    }

    curr->retain_enabled = false;
}

void 
cpu_end_interrupt() {
    cpu_t *curr = cpu_get_current();

    if (curr->retain_enabled) {
        printf("[error] retain enabled when ending an interrupt!\r\n");
        hcf();
    }

    curr->retain_enabled = true;
}

