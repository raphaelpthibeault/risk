#ifndef __CPU_APIC
#define __CPU_APIC

#include <types.h>
#include <cpu/cpuid.h>

#define LAPIC_SPURIOUS 0xF0
#define APIC_BASE_ADDR_MASK 0xFFFFF000

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100        // processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

void lapic_enable();


#endif /* ifndef __CPU_APIC */
