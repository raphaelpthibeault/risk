#pragma once 
#include <boot/limine/limine.h>
#include <stdbool.h>

typedef struct {
    uint64_t base;
    uint64_t length;
} memory_range_t;

inline 
memory_range_t
range_div(memory_range_t range, uint64_t val) {
    return (memory_range_t){ .base = range.base / val, .length = range.length / val};
}

typedef struct {
    uint8_t *data;
    uint64_t length;
} bitmap_t;

#define RESULT(ERROR, OK)   \
    struct {                \
        bool success;       \
        union {             \
            ERROR err;      \
            OK ok;          \
        };                  \
    }                       

#define OK(T, VALUE)        \
    (T) {                   \
        .success = true,    \
        .ok = VALUE         \
    }

#define ERR(T, ERROR)       \
    (T) {                   \
        .success = false,   \
        .ok = ERROR         \
    }


/* (should be) all syscall results */
#define FOREACH_RESULTS(RESULT) \
    RESULT(SUCCESS)             \
    RESULT(INTERRUPTED)         \
    RESULT(TIMEOUT)             \
    RESULT(ALREADY_BINDED)      \
    RESULT(ALREADY_ACK)         \
    RESULT(NOT_BINDED)          \
    RESULT(NOT_PERMITTED)       \
    RESULT(BAD_ADDRESS)         \
    RESULT(BAD_ARGUMENTS)       \
    RESULT(BAD_HANDLE)          \
    RESULT(BAD_ID)              \
    RESULT(BAD_SYSCALL)         \
    RESULT(NOT_IMPLEMENTED)     \
    RESULT(OUT_OF_MEMORY)       \
    RESULT(CHANNEL_FULL)        \
    RESULT(CHANNEL_EMPTY)       \
    RESULT(WOULD_BLOCK)


typedef enum {
#define ITER(result) RISK_##result,
    FOREACH_RESULTS(ITER)
#undef ITER 

    RISK_RESULT_COUNT
} risk_result_t;

typedef RESULT(risk_result_t, memory_range_t) PMM_RESULT;

#define PMM_USED true 
#define PMM_UNUSED false
#define BITMAP_SCALE 8 /* bit per byte */

void pmm_init(struct limine_memmap_response const *handover);

void pmm_bitmap_init(struct limine_memmap_response const *memory_map);

void pmm_mmap_load(struct limine_memmap_response const*memory_map);

PMM_RESULT pmm_unused(memory_range_t);

PMM_RESULT pmm_used(memory_range_t);

