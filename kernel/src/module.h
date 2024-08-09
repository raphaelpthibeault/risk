#pragma once 

#include <stdint.h>

typedef struct {
    void *header;
    void *body;
    uint64_t length;
} module_t;

