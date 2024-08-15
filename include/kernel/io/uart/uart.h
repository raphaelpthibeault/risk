#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#define SERIAL_COM1_PORT 0x3F8
#define SERIAL_COM2_PORT 0x2F8
#define SERIAL_COM3_PORT 0x3E8
#define SERIAL_COM4_PORT 0x2E8
#define SERIAL_COM5_PORT 0x5F8
#define SERIAL_COM6_PORT 0x4F8
#define SERIAL_COM7_PORT 0x5E8
#define SERIAL_COM8_PORT 0x4E8

bool serial_init_write();
void serial_write_str(const char *, uint16_t);

#endif /* ifndef UART_H */
