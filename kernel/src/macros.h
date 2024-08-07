#pragma once 



#define PACKED              __attribute__((packed))
#define ALWAYSINLINE        __attribute__((always_inline)) 

/* though of course cannot be used for free()-like functions directly, 
 * would have to do something like this (from systemmd)
 * https://github.com/systemd/systemd/blob/5809f340fd7e5e6c76e229059c50d92e1f57e8d8/src/basic/alloc-util.h#L50-L54
 * make the free function e.g. free pointer:
 * static inline void freep(void *p) {
 *     free(*(void**) p);
 * }
 *
 * #define CLEANUP_FREE _cleanup_(freep)
 * */
#define CLEANUP(FUNC)       __attribute__((cleanup(FUNC)))

#define _concat(LHS, RHS) LHS##RHS
#define CONCAT(LHS, RHS) _concat$(LHS, RHS)



#define ROUND_UP(n,mult)   ((n) % (mult) == 0 ? (n) : (n) - (n) % (mult) + (mult))
/* used for addresses
 * align upwards to nearest multiple:
 * e.g. 7 with align 10 -> 10
 *      10 with align 10 -> 10
 *      11 with align 10 -> 20
 * */
#define ALIGN_UP(num,align) (((num + (align)-1) & ~((align)-1)))
