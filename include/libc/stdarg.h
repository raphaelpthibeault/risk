#ifndef STDARG_H
#define STDARG_H

/* can cheat making the variadic args from scratch by leveraging the compilers...
 * */

typedef __builtin_va_list va_list;

#define va_start(v, l) __builtin_va_start(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_arg(v, type) __builtin_va_arg(v, type)

#endif /* ifndef STDARG_H */
