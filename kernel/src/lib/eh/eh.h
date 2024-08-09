#pragma once 

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

#define TRY(T, EXPR) ({             \
    typeof(EXPR) __expr = (EXPR);   \
    if (!__expr.success) {          \
        return ERR(T, __expr.err);  \
    }                               \
    __expr.ok;                      \
    })


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

