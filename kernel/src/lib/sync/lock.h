#pragma once 

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <macros.h>

typedef struct {
    atomic_int locked;
} lock_t;

bool lock_try_acquire(lock_t *); /* non-blocking */
void lock_acquire(lock_t *); /* blocking */
void lock_release(lock_t *);

