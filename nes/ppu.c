#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ppu.h"

static ppu_t *ppu = NULL;

void ppu_reset()
{
    assert(ppu);
    memset(ppu, 0, sizeof(ppu_t));
}

int ppu_init()
{
    ppu = malloc(sizeof(ppu_t));
    assert(ppu);
    ppu_reset();

    return 0;
}

void ppu_exit()
{
    assert(ppu);
    free(ppu);
    ppu = NULL;
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
            printf("READING FROM NON VALID ADDRESS IN PPU READ REG: 0x%04X\n", addr);
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
            printf("WRITING TO NON VALID ADDRESS IN PPU WRITE REG: 0x%04X\n", addr);
            assert(0);
            break;
    }
}