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

int nes_init()
{
    cpu_init();
    apu_init();
    ppu_init();
    mmu_init();

    nes_loadrom("testroms/nestest.nes");
    cpu_power_up();
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

int nes_loadrom(const char *path)
{
    /// TODO: Finish this. This is a quick function to load a test rom.

    assert(path);

    FILE * fp = fopen(path, "rb");
    assert(fp);

    fseek(fp, 0x10, SEEK_SET);

    mmu_t *mmu = mem_get_mem();
    assert(mmu);

    if (!fread(mmu->cart_mem, 0x4000, 1, fp))
    {
        goto jp;
    }

    memcpy(mmu->cart_mem_mirror, mmu->cart_mem, 0x4000);
    fclose(fp);
    return 0;

    jp:
    fclose(fp);
    assert(0);
    return -1;
}