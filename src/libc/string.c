#include <string.h>

void *
memchr(const void *s, int c, size_t n) {
	const unsigned char *ps;

	for (ps = s; 0 < n; ++ps, --n)
		if (*ps == c)
			return (void *)ps;
	return NULL;
}

int 
memcmp(const void *s, const void *t, size_t n) {
	const unsigned char *ps, *pt;
	for (ps = s, pt = t; n > 0; --n)
		if (*ps != *pt)
			return *ps < *pt ? -1 : 1;
	return 0;
}

void *
memcpy(void *d, const void *s, size_t n) {
	char *pd = d;
	const char *ps = s;
	while (n-- > 0)
		*pd++ = *ps++;
	return d;
}

void *
memmove(void *d, const void *s, size_t n) {
	char *pd = d;
	const char *ps = s;

	if (s > d)
		while (n-- > 0)
			*pd++ = *ps++;
	else
		for (ps += n, pd += n; 0 < n; --n)
			*pd-- = *ps--;
		
	return d;
}

void *
memset(void *p, int v, size_t n) {
	unsigned char *pp = p;
	while (n-- > 0)
		*pp++ = (unsigned char)v;

	return p;
}

