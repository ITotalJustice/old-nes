#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "cpu.h"
#include "mmu.h"
#include "apu.h"
#include "mmu.h"


int nes_init();
int nes_reset();
void nes_exit();

int nes_loadrom(const char *path);

#ifdef __cplusplus
}
#endif