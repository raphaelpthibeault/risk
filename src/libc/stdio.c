#include <stdio.h>
#include <stdlib.h>
#include <io/uart/uart.h>

int 
vsnprintf(char *buffer, size_t size, const char *format, va_list args) {
	char *str = buffer;
    const char *fmt = format;
    char num_buffer[32];

    while (*fmt && size > 1) {
        if (*fmt == '%') {
            ++fmt;
            switch (*fmt) {
                case 's': {
                    char* s = va_arg(args, char*);
                    while (*s && size > 1) {
                        *str++ = *s++;
                        --size;
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
                        --size;
                    }
                    break;
                }
                case 'l': {
                    ++fmt;
                    if (*fmt == 'l') {
                        ++fmt;
                        if (*fmt == 'u') {
                            uint64_t x = va_arg(args, uint64_t);
                            ulltoa(x, num_buffer, 10);
                            char* s = num_buffer;
                            while (*s && size > 1) {
                                *str++ = *s++;
                                --size;
                            }
                        }
                    }
                    break;
                }
                case 'x':
                case 'X':
                case 'p': {
                    uint64_t x = va_arg(args, uint64_t);
                    ulltoa(x, num_buffer, 16);
                    char* s = num_buffer;
                    while (*s && size > 1) {
                        *str++ = *s++;
                        --size;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    if (size > 1) {
                        *str++ = c;
                        --size;
                    }
                    break;
                }
                default:
                    break;
            }
        } else {
            if (size > 1) {
                *str++ = *fmt;
                --size;
            }
        }
        ++fmt;
    }

    *str = '\0';
    return str - buffer;
}

/* prints to uart */
int 
printf(const char *format, ...) {
	va_list args;
	va_start(args, format);

	char buf[256];
	vsnprintf(buf, sizeof(buf), format, args);

	serial_write_str(buf, SERIAL_COM1_PORT);

	va_end(args);
	return 0;
}


/* privates */

