#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "apu.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"

int nes_init()
{
    cpu_init();
    apu_init();
    ppu_init();
    mmu_init();

    return 0;
}

int nes_reset()
{
    cpu_reset();
    apu_reset();
    ppu_reset();
    mmu_reset();

    return 0;
}

void nes_exit()
{
    cpu_exit();
    apu_exit();
    ppu_exit();
    mmu_exit();
}