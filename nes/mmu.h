#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint8_t p;
} mmu_t;

void mmu_reset();
int mmu_init();
void mmu_exit();

#ifdef __cplusplus
}
#endif