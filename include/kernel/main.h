#ifndef KERNEL_MAIN_H
#define KERNEL_MAIN_H

#include <multiboot.h>
#include <stdint.h>

extern struct multiboot_tag *tag_start;

void kernel_start(unsigned long addr, unsigned long magic);

#endif /* ifndef KERNEL_MAIN_H */
