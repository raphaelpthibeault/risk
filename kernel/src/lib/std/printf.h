#pragma once 

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

char *itoa(uint64_t, char *, int);
int vsnprintf(char *, size_t, const char *, va_list);
void printf(const char *, ...);

