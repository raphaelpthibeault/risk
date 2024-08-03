#pragma once 

/* definition of atomic operations
 * implementations in assembly
 * */ 

#include <stdint.h>
#include <stdbool.h>

/* lock mutex @ bit position */
bool atomic_lock(uint64_t *mutex, uint64_t bit);
/* unlock mutex @ bit position */
bool atomic_unlock(uint64_t *mutex, uint64_t bit);
/* *attempt* to lock mutex @ bit position, will wait until acquired */
void atomic_spinlock(uint64_t *mutex, uint64_t bit);
/* accumulates acccumluator by amount, returns old acc */
int64_t atomic_accumulate(int64_t* acc, int64_t amount);

