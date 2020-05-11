#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint8_t p;
} apu_t;

void apu_reset();
int apu_init();
void apu_exit();

#ifdef __cplusplus
}
#endif