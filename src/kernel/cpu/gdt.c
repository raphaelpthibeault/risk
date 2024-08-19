#include <cpu/gdt.h>
#include <stdint.h>
#include <stdio.h>

/* idea:
 * 1 GDT per CPU
 * max 32
 * */

#define MAX_CPUS 32




/*
void tss_install(tss_entry_t *);
void tss_reload(void);
*/

//tss_t kernel_tss;


void 
tss_init(void) {
    printf("[info] initializing tss\r\n");

    /*
    // reserved fields set to 0 
    kernel_tss.reserved_0 = 0x00;
    kernel_tss.reserved_1 = 0x00;
    kernel_tss.reserved_2 = 0x00;
    kernel_tss.reserved_3 = 0x00;
    kernel_tss.reserved_4 = 0x00;

    // rsp[x] are used in the case of privilege change from a lower to a higher privilege
    // and contain the stack for that privilege level
    // We only use 0 and 3, so rsp1 and rsp2 are zeroed
    // Every thread will have its own rsp0 pointer
    kernel_tss.rsp[0] = (uint64_t)stack + 16384;
    kernel_tss.rsp[1] = 0x00;
    kernel_tss.rsp[2] = 0x00;

    // ist[x] are the interrupt stack table
    // see Chapter 5: Protection of the Intel Software Developer Manual, Volume 3-A 
    kernel_tss.ist[0] = 0x00;
    kernel_tss.ist[1] = 0x00;
    kernel_tss.ist[2] = 0x00;
    kernel_tss.ist[3] = 0x00;
    kernel_tss.ist[4] = 0x00;
    kernel_tss.ist[5] = 0x00;
    kernel_tss.ist[6] = 0x00;

    kernel_tss.iomap_base = 0x0;
    */
}

void 
tss_install(void) {
    /*
    // entry low 
    gdt.tss_lo = (tss_entry_lo_t){
        .limit_lo  = (uint16_t) sizeof(kernel_tss),         // 0-15, in theory should be 0xffff  
        .base_lo   = (uint64_t)&kernel_tss & 0xffff,        // 16-31, bits 0-15 of base i.e. kernel_tss address
        .base_mid  = (uint64_t)&kernel_tss & 0xff,          // 32-39, bits 16-23 of base
        .flags0    = 0x89,                                  // 40-47, type, 4 bits (1001), DPL = 0, P = 1
        .flags1    = 0,                                     // 48-55, limit (last 4 bits) can be 0, AVL=available to OS we leave it as 0, 53-54 are 0, 55 granularity
        .base_hi   = ((uint64_t)&kernel_tss >> 24) & 0xFF,  // 56-63, bits 24-31 of base
    };

    gdt.tss_hi = (tss_entry_hi_t){
        .base_highest = ((uint64_t)&kernel_tss >> 32) & 0xFFFFFFFF, // 64-95, base 32-63
        .reserved = 0,                                              // 96-127
    };

    // reload?
    */
}

