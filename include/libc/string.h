#ifndef STRING_H
#define STRING_H
#include <stddef.h>

void *memchr(const void *, int, size_t);
int   memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);

#endif /* ifndef STRING_H */
