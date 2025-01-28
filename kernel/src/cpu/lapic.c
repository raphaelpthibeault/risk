#include <types.h>
#include <cpu/lapic.h>
#include <cpu/cpuid.h>
#include <driver/serial.h>

uint32_t apic_base_addr;
uint64_t apic_hh_base_addr;
bool apic_in_X2_mode;

void lapic_enable() {
   uint64_t msr_output = rdmsr(IA32_APIC_BASE_MSR); 
   apic_base_addr = msr_output & APIC_BASE_ADDR_MASK; 

   /* TODO: map apic to higher half */
   /* TODO: Spurious Interrupt Vector */
   
   // check if X2APIC is available (unfortunately, it doesn't seem QEMU has it. Will have to use KVM later I think)
   uint32_t _, xapic_leaf = 0, x2apic_leaf = 0;
   cpuid(1, &_, &_, &x2apic_leaf, &xapic_leaf);
   (void)_;

   if (x2apic_leaf & (1 << 21)) {
      serial_printf("[Info], X2APIC is available\n");

   } else if (xapic_leaf & (1 << 9)) {
      serial_printf("[Info], APIC is available\n");


   } else {
      serial_printf("[ERROR], No Local APIC on this CPU\n");

      while (1);
   }

   
}


