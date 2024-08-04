#pragma once 

#include <io/io.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SERIAL_COM1_PORT 0x3F8
#define SERIAL_COM2_PORT 0x2F8
#define SERIAL_COM3_PORT 0x3E8
#define SERIAL_COM4_PORT 0x2E8
#define SERIAL_COM5_PORT 0x5F8
#define SERIAL_COM6_PORT 0x4F8
#define SERIAL_COM7_PORT 0x5E8
#define SERIAL_COM8_PORT 0x4E8


/* debug console */
//void serial_console_enable();
//void serial_print_error(const char*);
//void serial_print(const char*); /* implementation in shell.c */

bool serial_shell_enable();
void serial_write_str(const char*, uint16_t);

