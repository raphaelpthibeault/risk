#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <stdarg.h>
#include <stdint.h>

// Set the base revision to 2, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.

__attribute__((used, section(".requests")))
static volatile LIMINE_BASE_REVISION(2);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

// Framebuffer request
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// SMP request
__attribute__((used, section(".requests")))
static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

// Kernel Address request.
__attribute__((used, section(".requests")))
static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".requests_start_marker")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".requests_end_marker")))
static volatile LIMINE_REQUESTS_END_MARKER;

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *
memcpy(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *
memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *
memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int 
memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
#if defined (__x86_64__)
        asm ("hlt");
#elif defined (__aarch64__) || defined (__riscv)
        asm ("wfi");
#endif
    }
}

#define PORT 0x3f8          // COM1

static inline void 
outb(uint16_t port, uint8_t val) {
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t 
inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

static int 
init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(PORT + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}

int 
serial_received() {
   return inb(PORT + 5) & 1;
}

char 
read_serial() {
   while (serial_received() == 0);

   return inb(PORT);
}

int 
is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

void 
write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(PORT,a);
}

void 
write_serial_str(const char *str) {
    while (*str) {
        write_serial(*str++);
    }
}

static char * 
itoa(uint64_t value, char* buffer, int base) {
    char* ptr = buffer;
    char* ptr1 = buffer;
    char tmp_char;
    uint64_t tmp_value;

    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return buffer;
    }

    while (value != 0) {
        tmp_value = value % base;
        if (tmp_value < 10) {
            *ptr++ = (tmp_value + '0');
        } else {
            *ptr++ = (tmp_value - 10 + 'a');
        }
        value /= base;
    }

    *ptr = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *--ptr;
        *ptr = *ptr1;
        *ptr1++ = tmp_char;
    }
    return buffer;
}

static int 
vsnprintf(char* buffer, size_t size, const char* format, va_list args) {
    char* str = buffer;
    const char* fmt = format;
    char num_buffer[32];

    while (*fmt && size > 1) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's': {
                    char* s = va_arg(args, char*);
                    while (*s && size > 1) {
                        *str++ = *s++;
                        size--;
                    }
                    break;
                }
                case 'd':
                case 'i': {
                    int d = va_arg(args, int);
                    itoa(d, num_buffer, 10);
                    char* s = num_buffer;
                    while (*s && size > 1) {
                        *str++ = *s++;
                        size--;
                    }
                    break;
                }
                case 'l': {
                    fmt++;
                    if (*fmt == 'l') {
                        fmt++;
                        if (*fmt == 'u') {
                            uint64_t x = va_arg(args, uint64_t);
                            itoa(x, num_buffer, 10);
                            char* s = num_buffer;
                            while (*s && size > 1) {
                                *str++ = *s++;
                                size--;
                            }
                        }
                    }
                    break;
                }
                case 'x':
                case 'X':
                case 'p': {
                    uint64_t x = va_arg(args, uint64_t);
                    itoa(x, num_buffer, 16);
                    char* s = num_buffer;
                    while (*s && size > 1) {
                        *str++ = *s++;
                        size--;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (size > 1) {
                        *str++ = c;
                        size--;
                    }
                    break;
                }
                default:
                    break;
            }
        } else {
            if (size > 1) {
                *str++ = *fmt;
                size--;
            }
        }
        fmt++;
    }

    *str = '\0';
    return str - buffer;
}

void 
log_(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);

    write_serial_str(buf);

    va_end(args);
}

/* initialize the core kernel */
/* make smp right from the start */
void 
_start(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1 ||
        smp_request.response == NULL ||
        kernel_address_request.response == NULL) {
        hcf();
    }

    uint64_t processor_id = smp_request.response->bsp_lapic_id;
    uint64_t kernel_start = kernel_address_request.response->virtual_base;

    init_serial();
    log_("Processor ID: %d\n", processor_id);
    log_("Kernel Start: %llu\n", kernel_start);
    // higher half kernel so kernel_start should be 0xFFFFFFFF80000000 = 18446744071562067968
    int kstart_good = kernel_start == 0xFFFFFFFF80000000;
    log_("good kstart: %d\n", kstart_good);

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // Note: we assume the framebuffer model is RGB with 32-bit pixels.
    // draws a 0xffffff (white) line onto the screen
    for (size_t i = 0; i < 250; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    // We're done, just hang and freeze the emulator
    hcf();
}
