#pragma once 

#include <boot/protocol.h>
#include <lib/eh/eh.h>
#include <stdbool.h>
#include <stdint.h>

typedef RESULT(risk_result_t, memory_range_t) HEAP_RESULT;

HEAP_RESULT heap_alloc(uint64_t length);
HEAP_RESULT heap_alloc_lower(uint64_t length);
HEAP_RESULT heap_free(memory_range_t heap_range);
memory_range_t map_heap_range_to_pm_range(memory_range_t heap_range);

