#pragma once 


#pragma once 

#define PACKED              __attribute__((packed))
#define ALWAYSINLINE        __attribute__((always_inline)) 

#define ROUND_UP(n, mult)   ((n) % (mult) == 0 ? (n) : (n) - (n) % (mult) + (mult))

