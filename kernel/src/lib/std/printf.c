#include "printf.h"

char *
itoa(uint64_t value, char *buffer, int base) {
    char *ptr = buffer;
    char *ptr1 = buffer;
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

int
vsnprintf(char *buffer, size_t size, const char *format, va_list args) {
    char* str = buffer;
    const char* fmt = format;
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
                            itoa(x, num_buffer, 10);
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
                    itoa(x, num_buffer, 16);
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
