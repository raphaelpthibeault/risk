#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

char *itoa(int32_t value, char *str, int32_t base); // integer to a string
char *ulltoa(uint64_t value, char *buffer, int base); // unsigned long long to a string

#endif /* ifndef STDLIB_H */
