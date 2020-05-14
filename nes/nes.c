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

typedef struct
{
    const cpu_t *cpu;
    const apu_t *apu;
    const ppu_t *ppu;
    const mmu_t *mmu;
    const cart_t *cart;
} nes_t;

static nes_t nes = {0};
static bool nes_initialised = false;

int nes_init()
{
    assert(nes_initialised == false);
    if (nes_initialised)
    {
        fprintf(stderr, "nes already initialised\n");
        return -1;
    }

    nes.cpu = cpu_init();
    nes.apu = apu_init();
    nes.ppu = ppu_init();
    nes.mmu = mmu_init();
    nes.cart = cart_init();

    nes_initialised = true;

    return 0;
}

int nes_reset()
{
    assert(nes_initialised);
    if (nes_initialised == false)
    {
        fprintf(stderr, "nes not initialised\n");
        return -1;
    }

    cpu_reset();
    apu_reset();
    ppu_reset();
    mmu_reset();

    return 0;
}

void nes_exit()
{
    assert(nes_initialised);
    if (nes_initialised == false)
    {
        fprintf(stderr, "nes not initialised\n");
        return;
    }

    cpu_exit();
    apu_exit();
    ppu_exit();
    mmu_exit();
    cart_exit();

    nes_initialised = false;
}

int nes_loadrom(const char *path)
{
    assert(nes_initialised);
    if (nes_initialised == false)
    {
        fprintf(stderr, "nes not initialised\n");
        return -1;
    }

    assert(path);
    if (!path)
    {
        fprintf(stderr, "Empty path in rom load\n");
        return -1;
    }

    int ret = 0;

    ret = cart_load(path);
    assert(ret == 0);
    if (ret != 0)
    {
        return -1;
    }

    cpu_power_up();

    return 0;
}

#define NTSC_CYCLE 1789773 / 60

int nes_step()
{
    if (cpu_tick() != 0)
    {
        fprintf(stderr, "cpu tick error\n");
        return -1;
    }

    /// TODO:
    /// NTSC ppu updates 3 times exactly every 1 cpu cycle.
    /// However, each cpu instruction takes more than 1 cycle, 2 at min.
    /// So I can update this in the tick(), though i'll work on that later.
    for (int i = 0; i < 3; i++)
    {
        if (ppu_tick() != 0)
        {
            fprintf(stderr, "ppu tick error\n");
            return -1;
        }
    }

    return 0;
}

int nes_run()
{
    /// IMP: Not sure on this imp.
    while (nes.cpu->cycle < NTSC_CYCLE)
    {
        if (nes_step() != 0)
        {
            return -1;
        }
    }

    /// apu updates at 60hz, so as long as nes_run() is called at 60hz, everythign will be fine.
    if (apu_tick() != 0)
    {
        fprintf(stderr, "aputick error\n");
        return -1;
    }

    /// Will probably just make the structs rw instead of this.
    cpu_reset_cycle();

    return 0;
}