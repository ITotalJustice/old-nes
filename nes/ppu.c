#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ppu.h"
#include "mapper.h"

static ppu_t *ppu = NULL;

const ppu_t *ppu_init()
{
    assert(ppu == NULL);
    if (ppu)
    {
        fprintf(stderr, "ppu already initialised\n");
        return NULL;
    }

    ppu = malloc(sizeof(ppu_t));
    assert(ppu);
    if (!ppu)
    {
        fprintf(stderr, "Failed to alloc ppu\n");
        return NULL;
    }
    
    return ppu;
}

void ppu_exit()
{
    assert(ppu);
    if (!ppu)
    {
        fprintf(stderr, "ppu not initialised\n");
        return;
    }

    free(ppu);
    ppu = NULL;
}

int ppu_reset()
{
    assert(ppu);
    if (!ppu)
    {
        fprintf(stderr, "ppu not initialised\n");
        return -1;
    }

    memset(ppu, 0, sizeof(ppu_t));

    return 0;
}

uint8_t ppu_read_register(uint16_t addr)
{
    switch (addr)
    {
        case PPURegisterAddr_PPUCTRL:   return ppu->reg.ppu_ctrl;
        case PPURegisterAddr_PPUMASK:   return ppu->reg.ppu_mask;
        case PPURegisterAddr_PPUSTATUS: return ppu->reg.ppu_status;
        case PPURegisterAddr_OAMADDR:   return ppu->reg.oam_addr;
        case PPURegisterAddr_OAMDATA:   return ppu->reg.oam_data;
        case PPURegisterAddr_PPUSCROLL: return ppu->reg.ppu_scroll;
        case PPURegisterAddr_PPUADDR:   return ppu->reg.ppu_addr;
        case PPURegisterAddr_PPUDATA:   return ppu->reg.ppu_data;
        case PPURegisterAddr_OAMDMA:    return ppu->reg.oam_dma;

        default:
            fprintf(stderr, "READING FROM NON VALID ADDRESS IN PPU READ REG: 0x%04X\n", addr);
            assert(0);
            return 0;
    }
}

void ppu_write_register(uint16_t addr, uint8_t v)
{
    switch (addr)
    {
        case PPURegisterAddr_PPUCTRL:   ppu->reg.ppu_ctrl = v;      break;
        case PPURegisterAddr_PPUMASK:   ppu->reg.ppu_mask = v;      break;
        case PPURegisterAddr_PPUSTATUS: ppu->reg.ppu_status = v;    break;
        case PPURegisterAddr_OAMADDR:   ppu->reg.oam_addr = v;      break;
        case PPURegisterAddr_OAMDATA:   ppu->reg.oam_data = v;      break;
        case PPURegisterAddr_PPUSCROLL: ppu->reg.ppu_scroll = v;    break;
        case PPURegisterAddr_PPUADDR:   ppu->reg.ppu_addr = v;      break;
        case PPURegisterAddr_PPUDATA:   ppu->reg.ppu_data = v;      break;
        case PPURegisterAddr_OAMDMA:    ppu->reg.oam_dma = v;       break;
        default:
            fprintf(stderr, "WRITING TO NON VALID ADDRESS IN PPU WRITE REG: 0x%04X\n", addr);
            assert(0);
            break;
    }
}

typedef enum
{
    PPUMemMap_ST_PatternTable0,
    PPUMemMap_ED_PatternTable0 = 0x0FFF,
    PPUMemMap_ST_PatternTable1,
    PPUMemMap_ED_PatternTable1 = 0x1FFF,
    PPUMemMap_ST_Nametable0,
    PPUMemMap_ED_Nametable0 = 0x23FF,
    PPUMemMap_ST_Nametable1,
    PPUMemMap_ED_Nametable1 = 0x27FF,
    PPUMemMap_ST_Nametable2,
    PPUMemMap_ED_Nametable2 = 0x2BFF,
    PPUMemMap_ST_Nametable3,
    PPUMemMap_ED_Nametable3 = 0x2FFF,
    PPUMemMap_ST_NametableMirrors,
    PPUMemMap_ED_NametableMirrors = 0x3EFF,
    PPUMemMap_ST_PaletteRamIndexes,
    PPUMemMap_ED_PaletteRamIndexes = 0x3F1F,
    PPUMemMap_ST_PaletteRamIndexesMirrors,
    PPUMemMap_ED_PaletteRamIndexesMirrors = 0x3FFF,
} PPUMemMap;

static inline uint8_t ppu_read8(uint16_t addr)
{
    switch (addr)
    {
        case PPUMemMap_ST_PatternTable0 ... PPUMemMap_ED_PatternTable0: assert(0);
        case PPUMemMap_ST_PatternTable1 ... PPUMemMap_ED_PatternTable1: assert(0);

        case PPUMemMap_ST_Nametable0 ... PPUMemMap_ED_Nametable0: assert(0);
        case PPUMemMap_ST_Nametable1 ... PPUMemMap_ED_Nametable1: assert(0);
        case PPUMemMap_ST_Nametable2 ... PPUMemMap_ED_Nametable2: assert(0);
        case PPUMemMap_ST_Nametable3 ... PPUMemMap_ED_Nametable3: assert(0);

        case PPUMemMap_ST_NametableMirrors ... PPUMemMap_ED_NametableMirrors: assert(0);

        case PPUMemMap_ST_PaletteRamIndexes ... PPUMemMap_ED_PaletteRamIndexes: assert(0);
            switch (addr)
            {
                case PPUMemMap_ST_PaletteRamIndexes + 0:    return ppu->mem.palette_ram_indexes.background_colour;

                case PPUMemMap_ST_PaletteRamIndexes + 1:    return ppu->mem.palette_ram_indexes.background_palette0.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 2:    return ppu->mem.palette_ram_indexes.background_palette0.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 3:    return ppu->mem.palette_ram_indexes.background_palette0.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 5:    return ppu->mem.palette_ram_indexes.background_palette1.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 6:    return ppu->mem.palette_ram_indexes.background_palette1.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 7:    return ppu->mem.palette_ram_indexes.background_palette1.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 9:    return ppu->mem.palette_ram_indexes.background_palette2.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 10:   return ppu->mem.palette_ram_indexes.background_palette2.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 11:   return ppu->mem.palette_ram_indexes.background_palette2.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 13:   return ppu->mem.palette_ram_indexes.background_palette3.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 14:   return ppu->mem.palette_ram_indexes.background_palette3.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 15:   return ppu->mem.palette_ram_indexes.background_palette3.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 17:   return ppu->mem.palette_ram_indexes.sprite_palette0.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 18:   return ppu->mem.palette_ram_indexes.sprite_palette0.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 19:   return ppu->mem.palette_ram_indexes.sprite_palette0.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 21:   return ppu->mem.palette_ram_indexes.sprite_palette1.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 22:   return ppu->mem.palette_ram_indexes.sprite_palette1.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 23:   return ppu->mem.palette_ram_indexes.sprite_palette1.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 25:   return ppu->mem.palette_ram_indexes.sprite_palette2.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 26:   return ppu->mem.palette_ram_indexes.sprite_palette2.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 27:   return ppu->mem.palette_ram_indexes.sprite_palette2.colour[2];

                case PPUMemMap_ST_PaletteRamIndexes + 29:   return ppu->mem.palette_ram_indexes.sprite_palette3.colour[0];
                case PPUMemMap_ST_PaletteRamIndexes + 30:   return ppu->mem.palette_ram_indexes.sprite_palette3.colour[1];
                case PPUMemMap_ST_PaletteRamIndexes + 31:   return ppu->mem.palette_ram_indexes.sprite_palette3.colour[2];

                default: assert(0); return 0;
            }
        case PPUMemMap_ST_PaletteRamIndexesMirrors ... PPUMemMap_ED_PaletteRamIndexesMirrors:
            return ppu_read8(addr - (0x20 * ((addr - PPUMemMap_ST_PaletteRamIndexes) % 0x20)));
        
        default: assert(0); return 0;
    }
}

static inline void ppu_write8(uint16_t addr, uint8_t v)
{
    switch (addr)
    {
        case PPUMemMap_ST_PatternTable0 ... PPUMemMap_ED_PatternTable0: assert(0);
        case PPUMemMap_ST_PatternTable1 ... PPUMemMap_ED_PatternTable1: assert(0);

        case PPUMemMap_ST_Nametable0 ... PPUMemMap_ED_Nametable0: assert(0);
        case PPUMemMap_ST_Nametable1 ... PPUMemMap_ED_Nametable1: assert(0);
        case PPUMemMap_ST_Nametable2 ... PPUMemMap_ED_Nametable2: assert(0);
        case PPUMemMap_ST_Nametable3 ... PPUMemMap_ED_Nametable3: assert(0);

        case PPUMemMap_ST_NametableMirrors ... PPUMemMap_ED_NametableMirrors: assert(0);

        case PPUMemMap_ST_PaletteRamIndexes ... PPUMemMap_ED_PaletteRamIndexes: assert(0);
            switch (addr)
            {
                case PPUMemMap_ST_PaletteRamIndexes + 0:    ppu->mem.palette_ram_indexes.background_colour = v;             break;

                case PPUMemMap_ST_PaletteRamIndexes + 1:    ppu->mem.palette_ram_indexes.background_palette0.colour[0] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 2:    ppu->mem.palette_ram_indexes.background_palette0.colour[1] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 3:    ppu->mem.palette_ram_indexes.background_palette0.colour[2] = v; break;

                case PPUMemMap_ST_PaletteRamIndexes + 5:    ppu->mem.palette_ram_indexes.background_palette1.colour[0] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 6:    ppu->mem.palette_ram_indexes.background_palette1.colour[1] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 7:    ppu->mem.palette_ram_indexes.background_palette1.colour[2] = v; break;

                case PPUMemMap_ST_PaletteRamIndexes + 9:    ppu->mem.palette_ram_indexes.background_palette2.colour[0] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 10:   ppu->mem.palette_ram_indexes.background_palette2.colour[1] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 11:   ppu->mem.palette_ram_indexes.background_palette2.colour[2] = v; break;

                case PPUMemMap_ST_PaletteRamIndexes + 13:   ppu->mem.palette_ram_indexes.background_palette3.colour[0] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 14:   ppu->mem.palette_ram_indexes.background_palette3.colour[1] = v; break;
                case PPUMemMap_ST_PaletteRamIndexes + 15:   ppu->mem.palette_ram_indexes.background_palette3.colour[2] = v; break;

                case PPUMemMap_ST_PaletteRamIndexes + 17:   ppu->mem.palette_ram_indexes.sprite_palette0.colour[0] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 18:   ppu->mem.palette_ram_indexes.sprite_palette0.colour[1] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 19:   ppu->mem.palette_ram_indexes.sprite_palette0.colour[2] = v;     break;

                case PPUMemMap_ST_PaletteRamIndexes + 21:   ppu->mem.palette_ram_indexes.sprite_palette1.colour[0] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 22:   ppu->mem.palette_ram_indexes.sprite_palette1.colour[1] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 23:   ppu->mem.palette_ram_indexes.sprite_palette1.colour[2] = v;     break;

                case PPUMemMap_ST_PaletteRamIndexes + 25:   ppu->mem.palette_ram_indexes.sprite_palette2.colour[0] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 26:   ppu->mem.palette_ram_indexes.sprite_palette2.colour[1] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 27:   ppu->mem.palette_ram_indexes.sprite_palette2.colour[2] = v;     break;

                case PPUMemMap_ST_PaletteRamIndexes + 29:   ppu->mem.palette_ram_indexes.sprite_palette3.colour[0] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 30:   ppu->mem.palette_ram_indexes.sprite_palette3.colour[1] = v;     break;
                case PPUMemMap_ST_PaletteRamIndexes + 31:   ppu->mem.palette_ram_indexes.sprite_palette3.colour[2] = v;     break;

                default: assert(0); break;
            }
        case PPUMemMap_ST_PaletteRamIndexesMirrors ... PPUMemMap_ED_PaletteRamIndexesMirrors:
            ppu_write8(addr - (0x20 * ((addr - PPUMemMap_ST_PaletteRamIndexes) % 0x20)), v);
            break;
        
        default:
            assert(0);
            break;
    }
}

int ppu_tick()
{
    return 0;
}