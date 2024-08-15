#include <main.h>
#include <io/uart/uart.h>
#include <stdio.h>
#include <stdint.h>

void 
kernel_start(uint64_t addr, uint64_t magic) {
    serial_init_write();
    serial_write_str("\r\n", SERIAL_COM1_PORT);
    serial_write_str(
    "........      ...   ......   ..    ..\r\n"
    "..     ..    . .   ..        ..   ..\r\n"
    "..      ..   . .  ..         ..  ..\r\n"
    "..     ..    . .   ..        .. ..\r\n"
    "..   ..      . .    .....    ....\r\n"
    "..  ..       . .        ..   .. ..\r\n"
    "..    ..     . .         ..  ..  ..\r\n"
    "..     ..    . .        ..   ..   ..\r\n"
    "..      ..  ...    ......    ..    ..\r\n", SERIAL_COM1_PORT);


    printf("addr: %llu\r\n", addr);
    printf("magic: %llu\r\n", magic);


}


