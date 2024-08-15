#include <stdint.h>
#include <stdlib.h>

/* integer to a string */
char *
itoa(int32_t value, char *str, int32_t base) {
	// verify that the base if valid
	if (base < 2 || base > 36) { 
		*str = '\0'; 
		return str; 
	}

	char* ptr = str, *ptr1 = str, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return str;
}

/* unsigned long long to a string */
char *
ulltoa(uint64_t value, char *buffer, int base) {
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

    while (ptr1 < ptr) {
        tmp_char = *--ptr;
        *ptr = *ptr1;
        *ptr1++ = tmp_char;
    }
    return buffer;
}

