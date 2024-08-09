#pragma once 

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUM_CPUS 255

typedef struct {
    uint32_t id;
    bool active;
    bool retain_enabled;
    bool retain_depth;
    /* TODO tasking */
} cpu_t;

cpu_t *cpu_create(uint32_t id); /* creating the struct */
void cpu_initialize(cpu_t *); /* loading tss */
cpu_t *cpu_get_current(void);

/* it's the responsibility of the caller to be sure interrupt enable/disable is for the correct cpu */
void cpu_enable_interrupts(void);
void cpu_disable_interrupts(void);

void cpu_retain_disable();
void cpu_retain_enable();
void cpu_retain_interrupts();
void cpu_release_interrupts();

void cpu_being_interrupt(void);
void cpu_end_interrupt(void);

