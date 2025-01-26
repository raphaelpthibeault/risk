#ifndef __DRIVER_SERIAL
#define __DRIVER_SERIAL

#include <types.h>

#define PORT 0x3F8 /* COM1 */

uint8_t serial_init();
uint8_t serial_read();
void serial_write_str(const char *str);

#endif /* ifndef __DRIVER_SERIAL */
