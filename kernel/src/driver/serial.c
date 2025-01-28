#include <driver/serial.h>
#include <types.h>
#include <cpu/ports.h>
#include <lib/variadic.h>

static uint8_t is_transmit_empty();
static uint8_t serial_received();
static void serial_write(const uint8_t c);
static int serial_vasprintf(char *buf, const char *fmt, va_list args);
static size_t print_hex(unsigned long long val, uint32_t width, char *buf, int fill_zero, int align, int alt, int caps);
static size_t print_decimal(unsigned long long val, uint32_t width, char *buf, int fill_zero, int align, int precision);

uint8_t serial_init() {
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

uint8_t serial_read() {
    while (serial_received() == 0);
    return inb(PORT);
}

void serial_write_str(const char *str) {
    while (*str != '\0')
        serial_write(*str++);
}

int serial_printf(const char *fmt, ...) {
    char buf[1024];
    va_list args;

    va_start(args, fmt);
    int ret = serial_vasprintf(buf, fmt, args);
    va_end(args);
    serial_write_str(buf);

    return ret;
}

static uint8_t is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

static uint8_t serial_received() {
    return inb(PORT + 5) & 1;
}

static void serial_write(const uint8_t c) {
    while (is_transmit_empty() == 0);
    outb(PORT, c);
}

/* TODO: complete vasprintf */
static int serial_vasprintf(char *buf, const char *fmt, va_list args) {
    uint64_t written = 0;
    char *s;
    char *num;

    for (const char *f = fmt; *f; ++f) {
        if (*f != '%') {
            *buf++ = *f;
            ++written;
            continue;
        }

        ++f;
        
        uint32_t arg_width = 0;
        int big = 0;
        int align = 0; // to the left
        int always_sign = 0;
        int alt = 0;
        int fill_zero = 0;
        int precision = -1;

        while (1) {
			if (*f == '-') {
				align = 0;
				++f;
			} else if (*f == '#') {
				alt = 1;
				++f;
			} else if (*f == '*') {
				arg_width = (int)va_arg(args, int);
				++f;
			} else if (*f == '0') {
				fill_zero = 1;
				++f;
			} else if (*f == '+') {
				always_sign = 1;
				++f;
			} else if (*f == ' ') {
				always_sign = 2;
				++f;
			} else {
				break;
			}
		}
        while (*f >= '0' && *f <= '9') {
			arg_width *= 10;
			arg_width += *f - '0';
			++f;
		}
        if (*f == '.') {
			++f;
			precision = 0;
			if (*f == '*') {
				precision = (int)va_arg(args, int);
				++f;
			} else  {
				while (*f >= '0' && *f <= '9') {
					precision *= 10;
					precision += *f - '0';
					++f;
				}
			}
		}
        if (*f == 'l') {
            big = 1;
            ++f;
            if (*f == 'l') {
                big = 2;
                ++f;
            }
        }
        if (*f == 'j') {
            big = (sizeof(uintmax_t) == sizeof(unsigned long long) ? 2 :
				   sizeof(uintmax_t) == sizeof(unsigned long) ? 1 : 0);
			++f;
        }
        if (*f == 'z') {
			big = (sizeof(size_t) == sizeof(unsigned long long) ? 2 :
				   sizeof(size_t) == sizeof(unsigned long) ? 1 : 0);
			++f;
		}
		if (*f == 't') {
			big = (sizeof(ptrdiff_t) == sizeof(unsigned long long) ? 2 :
				   sizeof(ptrdiff_t) == sizeof(unsigned long) ? 1 : 0);
			++f;
		}

        switch (*f) {
            case 's': /* string pointer to a string */
                {
                    uint64_t count = 0;

                    if (big) {
                        return written;
                    } else {
                        s = (char *)va_arg(args, char*);
                        if (s == NULL) 
                            s = "(NULL)";
                        // won't support precision for now
                        while (*s) {
                            *buf++ = *s++;
                            ++written;
                            ++count;
                            if (arg_width && count == arg_width) break;
                        }
                        
                    }

                    while (count < arg_width) {
                        *buf++ = ' ';
                        ++written;
                        ++count;
                    }
                }
                break;
            case 'c': 
                { /* single character */
                    char c = (char)va_arg(args, int);
                    *buf++ = c;
                    ++written;
                }
                break;
            case 'p':
                alt = 1;
                if (sizeof(void*) == sizeof(long long)) big = 2; // fall through
            case 'X': // fall through
            case 'x': // hex number
                {
                    char tmp[100];
                    unsigned long long val;

                    if (big == 2) {
						val = (unsigned long long)va_arg(args, unsigned long long);
					} else if (big == 1) {
						val = (unsigned long)va_arg(args, unsigned long);
					} else {
						val = (unsigned int)va_arg(args, unsigned int);
					}

                    written += print_hex(val, arg_width, tmp, fill_zero, align, alt, !(*f & 32));

                    num = tmp;
                    while (*num) {
                        *buf++ = *num++;
                    }
                }
                break;
            case 'i': // fall through
            case 'd': // decimal number
                {
                    char tmp[100];
                    long long val;
                    if (big == 2) {
						val = (long long)va_arg(args, long long);
					} else if (big == 1) {
						val = (long)va_arg(args, long);
					} else {
						val = (int)va_arg(args, int);
					}
                    if (val < 0) {
                        *buf++ = '-';
                        ++written;
                        val = -val;
                    } else if (always_sign) {
                        *buf++ = always_sign == 2 ? ' ' : '+';
                        ++written;
                    }

                    written += print_decimal(val, arg_width, tmp, fill_zero, align, precision);

                    num = tmp;
                    while (*num) {
                        *buf++ = *num++;
                    }
                }
                break;
            case 'u': // unsigned decimal number
                {
                    char tmp[100];
                    unsigned long long val;

					if (big == 2) {
						val = (unsigned long long)va_arg(args, unsigned long long);
					} else if (big == 1) {
						val = (unsigned long)va_arg(args, unsigned long);
					} else {
						val = (unsigned int)va_arg(args, unsigned int);
					}

                    written += print_decimal(val, arg_width, tmp, fill_zero, align, precision);

                    num = tmp;
                    while (*num) {
                        *buf++ = *num++;
                    }
                }
                break;
            default:
                break;
        }
    }


    return written;
}

static size_t print_hex(unsigned long long val, uint32_t width, char *buf, int fill_zero, int align, int alt, int caps) {
    size_t written = 0;
    uint32_t i = width;

    unsigned long long input_width = 1;
    unsigned long long j = 0x0F;
    while (val > j && j < UINT64_MAX) {
        input_width += 1;
        j *= 0x10;
        j += 0x0F;
    }

    if (align == 1) { // aligned to the right
        while (i > input_width + 2*!!alt) {
            *buf++ = fill_zero ? '0' : ' ';
            ++written;
            --i;
        }
    }

    if (alt) {
        *buf++ = '0';
        ++written;

        *buf++ = caps ? 'X' : 'x';
    }

    i = input_width;
    while (i-- > 0) {
		char c = (caps ? "0123456789ABCDEF" : "0123456789abcdef")[(val>>(i*4))&0xF];
        *buf++ = c;
        ++written;
    }

    if (align == 0) {
        i = width;
        while (i > input_width + 2*!!alt) {
            *buf++ = fill_zero ? '0' : ' ';
            ++written;
            i--;
        }
    }

    return written;
}
/* vasprintf will print the negative sign if the original val is negative 
 * we don't have to deal with a negative number here */
static size_t print_decimal(unsigned long long val, uint32_t width, char *buf, int fill_zero, int align, int precision) {
    size_t written = 0;
    unsigned long long input_width = 1;
	unsigned long long i;

	if (precision == -1) 
        precision = 1;

    if (val == 0) {
        input_width = 0;
    } else {
        unsigned long long tmp = val;
        while (tmp >= 10UL) {
            tmp /= 10UL;
            ++input_width;
        }
    }
    
    if (input_width < (unsigned long long)precision)
        input_width = precision;

    int count = 0;
    i = input_width;
    char tmp[100];

    if (align) { // align right
        while (input_width + count < width) {
            *buf++ = fill_zero ? '0' : ' ';
            ++written;
            ++count;
        }
        
        while (i > 0) {
            unsigned long long n = val / 10;
            long long r = val % 10;
            tmp[i-1] = r + '0';
            --i;
            val = n;
        }

        while (i < input_width) {
            *buf++ = tmp[i];
            ++written;
            ++i;
        }
    } else { // align left
        while (i > 0) {
            unsigned long long n = val / 10;
            long long r = val % 10;
            tmp[i-1] = r + '0';
            --i;
            val = n;
            ++count;
        }

        while (i < input_width) {
            *buf++ = tmp[i];
            ++written;
            ++i;
        }

        while (count < width) {
            *buf++ = fill_zero ? '0' : ' ';
            ++written;
            count++;
        }
    }

    return written;
}

