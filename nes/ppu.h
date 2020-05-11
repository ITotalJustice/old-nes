#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{

} ppu_registers_t;

typedef struct
{
    ppu_registers_t reg;
} ppu_t;


void ppu_reset();
int ppu_init();
void ppu_exit();

#ifdef __cplusplus
}
#endif