#include "lock.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include "sync.h"

bool 
lock_try_acquire(lock_t *lock) {
    sync_enter();

    bool result; 
    int expected = false;

    /* try acquire */
    result = atomic_compare_exchange_strong(&lock->locked, &expected, true);
    atomic_thread_fence(memory_order_seq_cst);


    if (!result) {
        sync_leave();
    }

    return result;
}

void 
lock_acquire(lock_t *lock) {
    sync_enter();

    while (!lock_try_acquire(lock)) {
        #ifdef HAS_PAUSE
        sync_pause();
        #endif
        atomic_thread_fence(memory_order_seq_cst);
    }

    atomic_thread_fence(memory_order_seq_cst);
}

void 
lock_release(lock_t *lock) {
    atomic_thread_fence(memory_order_seq_cst);
    lock->locked = false;
    sync_leave();
}

