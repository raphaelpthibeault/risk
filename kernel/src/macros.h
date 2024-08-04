#pragma once 

#define PACKED              __attribute__((packed))
/* round up n to the nearest multiple */
#define ROUND_UP(n, mult)   ((n) % (mult) == 0 ? (n) : (n) - (n) % (mult) + (mult))

