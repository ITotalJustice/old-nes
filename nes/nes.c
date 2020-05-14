#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "nes.h"
#include "apu.h"
#include "cpu.h"
#include "mmu.h"
#include "ppu.h"
#include "cart.h"

int nes_init()
{
    cpu_init();
    apu_init();
    ppu_init();
    mmu_init();
    cart_init();

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
    cart_exit();
}

int nes_loadrom(const char *path)
{
    /// TODO: Finish this.
    assert(path);
    if (!path)
    {
        fprintf(stderr, "Empty path in rom load\n");
        return -1;
    }

    int ret = 0;

    ret = cart_load(path);
    assert(ret == 0);
    if (ret != 0) return -1;

    cpu_power_up();

    return 0;
}