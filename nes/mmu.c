#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mmu.h"
#include "apu.h"
#include "cpu.h"
#include "ppu.h"

static mmu_t *mmu = NULL;

void mmu_reset()
{
    assert(mmu);
    memset(mmu, 0, sizeof(mmu_t));
}

int mmu_init()
{
    mmu = malloc(sizeof(mmu_t));
    assert(mmu);
    mmu_reset();

    return 0;
}

void mmu_exit()
{
    assert(mmu);
    free(mmu);
    mmu = NULL;
}

uint8_t mmu_read8(uint16_t addr)
{
    switch (addr)
    {
        case 0x0000 ... 0x07FF:
            return mmu->ram[addr];
        
        case 0x0800 ... 0x1FFF:
            return mmu->mirror_ram[addr - 0x800];
        
        /// ppu reg
        case 0x2000 ... 0x2007:
            printf("ppu reg read\n");
            assert(0);

        /*
        /// ppu reg mirrored...alot
        case 0x2008 ... 0x3FFF:
            printf("ppu reg mirror read\n");
            assert(0);

        /// sound / joypad / io
        case 0x4000 ... 0x401F:
            printf("sound joy io read\n");
            assert(0);
        */

        /// cart prg-ram OR prg-rom
        case 0x6000 ... 0x7FFF:
            return mmu->cart_ram[addr - 0x6000];

        /// cart mem
        case 0x8000 ... 0xBFFF:
            return mmu->cart_mem[addr - 0x8000];

        case 0xC000 ... 0xFFFF:
            return mmu->cart_mem_mirror[addr - 0xC000];

        default:
            printf("UNKOWN READ MEM ADDRESS 0x%X\n", addr);
            assert(0);
            return 0;
    }
}

uint16_t mmu_read16(uint16_t addr)
{
    return (mmu_read8(addr + 1) << 8) | mmu_read8(addr); 
}

void mmu_write8(uint16_t addr, uint8_t v)
{
    switch (addr)
    {
        case 0x0000 ... 0x07FF:
            mmu->ram[addr] = v;
            mmu->mirror_ram[addr] = v;
            break;
        
        case 0x0800 ... 0x1FFF:
            mmu->mirror_ram[addr - 0x800] = v;
            mmu->ram[addr - 0x800] = v;
            break;

        /*
        /// ppu reg
        case 0x2000 ... 0x2007:
            break;

        /// ppu reg mirrored...alot
        case 0x2008 ... 0x3FFF:
            break;

        /// sound / joypad / io
        case 0x4000 ... 0x401F:
            break;
            */

        /// cart prg-ram OR prg-rom
        case 0x6000 ... 0x7FFF:
            mmu->cart_ram[addr - 0x6000] = v;
            break;

        /// cart mem
        case 0x8000 ... 0xBFFF:
            mmu->cart_mem[addr - 0x8000] = v;
            break;

        case 0xC000 ... 0xFFFF:
            mmu->cart_mem_mirror[addr - 0xC000] = v;
            break;

        default:
            printf("UNKOWN WRITE MEM ADDRESS 0x%X\n", addr);
            assert(0);
    }
}

void mmu_write16(uint16_t addr, uint16_t v)
{
    mmu_write8(addr, v & 0xFF);
    mmu_write8(addr + 1, v >> 8);
}

mmu_t *mem_get_mem()
{
    return mmu;
}