#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mmu.h"
#include "apu.h"
#include "cpu.h"
#include "ppu.h"
#include "cart.h"

static mmu_t *mmu = NULL;

const mmu_t *mmu_init()
{
    assert(mmu == NULL);
    if (mmu)
    {
        fprintf(stderr, "mmu already initialised\n");
        return NULL;
    }

    mmu = malloc(sizeof(mmu_t));
    assert(mmu);
    if (!mmu)
    {
        fprintf(stderr, "Failed to alloc mmu\n");
        return NULL;
    }
    
    return mmu;
}

void mmu_exit()
{
    assert(mmu);
    if (!mmu)
    {
        fprintf(stderr, "mmu not initialised\n");
        return;
    }

    free(mmu);
    mmu = NULL;
}

int mmu_reset()
{
    assert(mmu);
    if (!mmu)
    {
        fprintf(stderr, "mmu not initialised\n");
        return -1;
    }

    memset(mmu, 0, sizeof(mmu_t));

    return 0;
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

typedef enum
{
    CPURegMemMap_SQ1_VOL = 0x4000,
    CPURegMemMap_SQ1_SWEEP,
    CPURegMemMap_SQ1_LO,
    CPURegMemMap_SQ1_HI,
    CPURegMemMap_SQ2_VOL,
    CPURegMemMap_SQ2_SWEEP,
    CPURegMemMap_SQ2_LO,
    CPURegMemMap_SQ2_HI,
    CPURegMemMap_TRI_LINEAR,
    CPURegMemMap_unused0,
    CPURegMemMap_TRI_LO,
    CPURegMemMap_TRI_HI,
    CPURegMemMap_NOISE_VOL,
    CPURegMemMap_unused1,
    CPURegMemMap_NOISE_LO,
    CPURegMemMap_NOISE_HI,
    CPURegMemMap_DMC_FREQ,
    CPURegMemMap_DMC_RAW,
    CPURegMemMap_DMC_START,
    CPURegMemMap_DMC_LEN,
    CPURegMemMap_OAMDMA,
    CPURegMemMap_SND_CHN,
    CPURegMemMap_JOY1,
    CPURegMemMap_JOY2,
} CPURegMemMap;

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

        /// sound / joypad / io
        case 0x4000 ... 0x401F:
            switch (addr)
            {
                case CPURegMemMap_SQ1_VOL:      return apu_read_register(addr);
                case CPURegMemMap_SQ1_SWEEP:    return apu_read_register(addr);
                case CPURegMemMap_SQ1_LO:       return apu_read_register(addr);
                case CPURegMemMap_SQ1_HI:       return apu_read_register(addr);
                case CPURegMemMap_SQ2_VOL:      return apu_read_register(addr);
                case CPURegMemMap_SQ2_SWEEP:    return apu_read_register(addr);
                case CPURegMemMap_SQ2_LO:       return apu_read_register(addr);
                case CPURegMemMap_SQ2_HI:       return apu_read_register(addr);
                case CPURegMemMap_TRI_LINEAR:   return apu_read_register(addr);
                case CPURegMemMap_unused0:      return apu_read_register(addr);
                case CPURegMemMap_TRI_LO:       return apu_read_register(addr);
                case CPURegMemMap_TRI_HI:       return apu_read_register(addr);
                case CPURegMemMap_NOISE_VOL:    return apu_read_register(addr);
                case CPURegMemMap_unused1:      return apu_read_register(addr);
                case CPURegMemMap_NOISE_LO:     return apu_read_register(addr);
                case CPURegMemMap_NOISE_HI:     return apu_read_register(addr);
                case CPURegMemMap_DMC_FREQ:     return apu_read_register(addr);
                case CPURegMemMap_DMC_RAW:      return apu_read_register(addr);
                case CPURegMemMap_DMC_START:    return apu_read_register(addr);
                case CPURegMemMap_DMC_LEN:      return apu_read_register(addr);
                case CPURegMemMap_OAMDMA:       return ppu_read_register(addr);
                case CPURegMemMap_SND_CHN:      return apu_read_register(addr);
                case CPURegMemMap_JOY1:         return 0; /// joy1
                case CPURegMemMap_JOY2:         return 0; /// joy2
                default:
                    fprintf(stderr, "READING UNSUED MEM MAPPED REGISTERS 0x%04X\n", addr);
                    assert(0);
            }

        /// cart prg-ram OR prg-rom
        case 0x6000 ... 0x7FFF:
            return cart_read(addr - 0x6000);

        /// cart mem
        case 0x8000 ... 0xBFFF:
            return cart_read(addr - 0x8000);

        case 0xC000 ... 0xFFFF:
            return cart_read(addr - 0xC000);

        default:
            fprintf(stderr, "UNKOWN READ MEM ADDRESS 0x%X\n", addr);
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

        /// sound / joypad / io
        case 0x4000 ... 0x401F:
            switch (addr)
            {
                case CPURegMemMap_SQ1_VOL:      apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ1_SWEEP:    apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ1_LO:       apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ1_HI:       apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ2_VOL:      apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ2_SWEEP:    apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ2_LO:       apu_write_register(addr, v);    break;
                case CPURegMemMap_SQ2_HI:       apu_write_register(addr, v);    break;
                case CPURegMemMap_TRI_LINEAR:   apu_write_register(addr, v);    break;
                case CPURegMemMap_unused0:      apu_write_register(addr, v);    break;
                case CPURegMemMap_TRI_LO:       apu_write_register(addr, v);    break;
                case CPURegMemMap_TRI_HI:       apu_write_register(addr, v);    break;
                case CPURegMemMap_NOISE_VOL:    apu_write_register(addr, v);    break;
                case CPURegMemMap_unused1:      apu_write_register(addr, v);    break;
                case CPURegMemMap_NOISE_LO:     apu_write_register(addr, v);    break;
                case CPURegMemMap_NOISE_HI:     apu_write_register(addr, v);    break;
                case CPURegMemMap_DMC_FREQ:     apu_write_register(addr, v);    break;
                case CPURegMemMap_DMC_RAW:      apu_write_register(addr, v);    break;
                case CPURegMemMap_DMC_START:    apu_write_register(addr, v);    break;
                case CPURegMemMap_DMC_LEN:      apu_write_register(addr, v);    break;
                case CPURegMemMap_OAMDMA:       ppu_write_register(addr, v);    break;
                case CPURegMemMap_SND_CHN:      apu_write_register(addr, v);    break;
                case CPURegMemMap_JOY1:         assert(0); break; /// joystick strobe.
                case CPURegMemMap_JOY2:         apu_write_register(addr, v);    break;
                default:
                    fprintf(stderr, "READING UNSUED MEM MAPPED REGISTERS 0x%04X\n", addr);
                    assert(0);
                    break;
            }

        /// cart READ ONLY
        case 0x6000 ... 0xFFFF:
            fprintf(stderr, "Trying to write to ROM cart: 0x%X\n", addr);
            assert(0);
            break;

        default:
            fprintf(stderr, "UNKOWN WRITE MEM ADDRESS 0x%X\n", addr);
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