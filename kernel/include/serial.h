#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <io.h>

#ifdef __x86_64
#define PORT 0x3f8          // COM1
#endif
// riscv64 should be 0x10000000, aarch64 idk. A problem for later

int init_serial();
void write_serial(char a);
void write_serial_str(const char *str);

