#include <io/uart/uart.h>
#include <io/io.h>

static uint16_t _input_port;
static uint16_t _output_port;
static uint8_t _available_com_ports[8] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
};

static void serial_select_input_port(uint16_t);
static void serial_select_output_port(uint16_t);
static bool serial_check_port(uint16_t);
static bool serial_configure_port(uint16_t);
/*
static int serial_received(uint16_t);
static char serial_read(uint16_t);
*/
static int is_transmit_empty(uint16_t);
static void serial_write(char, uint16_t);

/* initializes COM1 serial port
 * */
bool 
serial_init_write() {
    _input_port = 0;
    _output_port = 0;
    uint16_t i;

    for (i = 1; i < 9; ++i) {
        if (serial_check_port(i)) {
            _available_com_ports[i] = true;
            serial_configure_port(i);
        }
    }
    for (i = 1; i < 9; ++i) {
        if (_available_com_ports[i]) {
            serial_select_output_port(i);
            break;
        }
    }
    for (i = 1; i < 3; ++i) { // curiously only COM1 and COM2 seem to work as input ports (at least with QEMU)
        if (_available_com_ports[i]) {
            serial_select_input_port(i);
            break;
        }
    }

    return _input_port != 0 && _output_port != 0;
}

void 
serial_write_str(const char * str, uint16_t port) {
    while (*str) {
        serial_write(*str++, port);
    }
}

/* privates */

static void 
serial_select_input_port(uint16_t port) {
    if (port == 0 || port > 8)
        return;

    _input_port = port;
}

static void 
serial_select_output_port(uint16_t port) {
    if (port == 0 || port > 8) 
        return;

    _output_port = port;
}

static bool 
serial_check_port(uint16_t port) {
    if (port == 0 || port > 8) 
        return false;

    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    outb(port + 0, 0xAE);
    return inb(port + 0) == 0xAE;
}

static bool
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

/*
static int
serial_received(uint16_t port) {
   return inb(port + 5) & 1;
}


static char
read_serial(uint16_t port) {
   while (serial_received(port) == 0);

   return inb(port);
}
*/

static int
is_transmit_empty(uint16_t port) {
   return inb(port + 5) & 0x20;
}

static void
serial_write(char a, uint16_t port) {
   while (is_transmit_empty(port) == 0);

   outb(port,a);
}

