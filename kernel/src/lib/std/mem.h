#pragma once 

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memmove(void *, const void *, size_t);
int memcmp(const void *, const void *, size_t);

