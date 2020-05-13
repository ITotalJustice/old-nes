#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mmu.h"
#include "apu.h"
#include "cpu.h"
#include "ppu.h"

mmu_t *mmu = NULL;

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

typedef enum
{
    CPUMemMap_ST_Ram,
    CPUMemMap_ED_Ram            = 0x07FF,

    CPUMemMap_ST_RamMirror,
    CPUMemMap_ED_RamMirror      = 0x1FFF,

    CPUMemMap_ST_PPUReg,
    CPUMemMap_ED_PPUReg         = 0x2007,

    CPUMemMap_ST_PPURegMirror,
    CPUMemMap_ED_PPURegMirror   = 0x3FFF,
} CPUMemMap;

uint8_t mmu_read8(uint16_t addr)
{
    switch (addr)
    {    
        case CPUMemMap_ST_Ram ... CPUMemMap_ED_Ram:
            return mmu->ram[addr];
        
        case CPUMemMap_ST_RamMirror ... CPUMemMap_ED_RamMirror:
            return mmu->ram[addr - (CPUMemMap_ST_RamMirror * (addr % CPUMemMap_ST_RamMirror))];
        
        /// ppu reg
        case CPUMemMap_ST_PPUReg ... CPUMemMap_ED_PPUReg:
            return ppu_read_register(addr);

        /// ppu reg mirrored...alot
        case CPUMemMap_ST_PPURegMirror ... CPUMemMap_ED_PPURegMirror:
            return ppu_read_register(addr - (0x8 * ((addr - CPUMemMap_ST_PPUReg) % 0x8)));

        /*
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
            return mmu->cart_mem[addr - 0xC000];

        default:
            printf("UNKOWN READ MEM ADDRESS 0x%X\n", addr);
            assert(0);
            return 0;
    }
}

uint16_t mmu_read16(uint16_t addr)
{
    return (mmu_read8(addr)) | (mmu_read8(addr + 1) << 8); 
}

void mmu_write8(uint16_t addr, uint8_t v)
{
    switch (addr)
    {
        case CPUMemMap_ST_Ram ... CPUMemMap_ED_Ram:
            mmu->ram[addr] = v;
            break;
        
        case CPUMemMap_ST_RamMirror ... CPUMemMap_ED_RamMirror:
            mmu->ram[addr - (CPUMemMap_ST_RamMirror * (addr % CPUMemMap_ST_RamMirror))] = v;
            break;
        
        /// ppu reg
        case CPUMemMap_ST_PPUReg ... CPUMemMap_ED_PPUReg:
            ppu_write_register(addr, v);
            break;

        /// ppu reg mirrored...alot
        case CPUMemMap_ST_PPURegMirror ... CPUMemMap_ED_PPURegMirror:
            ppu_write_register(addr - (0x8 * ((addr - CPUMemMap_ST_PPUReg) % 0x8)), v);
            break;

        /*
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
            mmu->cart_mem[addr - 0xC000] = v;
            break;

        default:
            printf("UNKOWN WRITE MEM ADDRESS 0x%X\n", addr);
            assert(0);
            break;
    }
}

void mmu_write16(uint16_t addr, uint16_t v)
{
    /// LSB first then MSB.
    mmu_write8(addr, v & 0xFF);
    mmu_write8(addr + 1, v >> 8);
}

mmu_t *mem_get_mem()
{
    return mmu;
}