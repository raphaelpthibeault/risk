#ifndef __DRIVER_SERIAL
#define __DRIVER_SERIAL

#include <types.h>
#include <lib/variadic.h>

#define PORT 0x3F8 /* COM1 */

uint8_t serial_init();
uint8_t serial_read();
void serial_write_str(const char *str);
int serial_printf(const char *, ...);

#endif /* ifndef __DRIVER_SERIAL */
