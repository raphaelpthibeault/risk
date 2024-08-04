#include "uart.h"
#include <stdbool.h>
#include <stdint.h>

static uint16_t input_port; // going to write an input eventually, but I think I need apic to truly write something that works 
static uint16_t output_port;

static uint8_t available_com_ports[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

uint16_t __com_port_addrs[9] = {
    0,
    SERIAL_COM1_PORT,
    SERIAL_COM2_PORT,
    SERIAL_COM3_PORT,
    SERIAL_COM4_PORT,
    SERIAL_COM5_PORT,
    SERIAL_COM6_PORT,
    SERIAL_COM7_PORT,
    SERIAL_COM8_PORT
};

static void 
serial_select_input_port(uint16_t port) {
    if (port == 0 || port > 8)
        return;

    input_port = port;
}

static void 
serial_select_output_port(uint16_t port) {
    if (port == 0 || port > 8) 
        return;

    output_port = port;
}

bool 
serial_check_port(uint16_t port) {
    if (port == 0 || port > 8) 
        return false;

    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    outb(port + 0, 0xAE);
    return inb(port + 0) == 0xAE;
}

bool
serial_configure_port(uint16_t port) {
    outb(port + 1, 0x00);    // Disable all interrupts
    outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);    //                  (hi byte)
    outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    return true;
}

bool 
serial_shell_enable() {
    input_port = 0;
    output_port = 0;
    uint16_t i;

    for (i = 1; i < 9; ++i) {
        if (serial_check_port(i)) {
            available_com_ports[i] = true;
            serial_configure_port(i);
        }
    }
    for (i = 1; i < 9; ++i) {
        if (available_com_ports[i]) {
            serial_select_output_port(i);
            break;
        }
    }
    for (i = 1; i < 3; ++i) { // curiously only COM1 and COM2 seem to work as input ports
        if (available_com_ports[i]) {
            serial_select_input_port(i);
            break;
        }
    }

    return input_port != 0 && output_port != 0;
}


int
serial_received(uint16_t port) {
   return inb(port + 5) & 1;
}

char
read_serial(uint16_t port) {
   while (serial_received(port) == 0);

   return inb(port);
}

int
is_transmit_empty(uint16_t port) {
   return inb(port + 5) & 0x20;
}

void
serial_write(char a, uint16_t port) {
   while (is_transmit_empty(port) == 0);

   outb(port,a);
}

void
serial_write_str(const char *str, uint16_t port) {
    while (*str) {
        serial_write(*str++, port);
    }
}



