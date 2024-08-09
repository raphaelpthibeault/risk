#pragma once 
#include <boot/limine/limine.h>
#include <stdbool.h>
#include <lib/eh/eh.h>
#include <boot/protocol.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    uint64_t length;
} bitmap_t;

typedef RESULT(risk_result_t, memory_range_t) PMM_RESULT;

#define PMM_USED true 
#define PMM_UNUSED false
#define BITMAP_SCALE 8 /* bit per byte */

void pmm_init(struct limine_memmap_response const *handover);

void pmm_bitmap_init(struct limine_memmap_response const *memory_map);

void pmm_mmap_load(struct limine_memmap_response const*memory_map);

PMM_RESULT pmm_unused(memory_range_t);

PMM_RESULT pmm_used(memory_range_t);

PMM_RESULT pmm_alloc(uint64_t length, bool upper);

