#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "cpu.h"
#include "apu.h"
#include "ppu.h"
#include "cart.h"


int nes_init();
void nes_exit();

int nes_reset();

int nes_loadrom(const char *path);

int nes_run();
int nes_step();

#ifdef __cplusplus
}
#endif