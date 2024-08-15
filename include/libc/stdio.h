#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *, size_t, const char *, va_list);
int printf(const char *, ...);

#endif /* ifndef STDIO_H */
