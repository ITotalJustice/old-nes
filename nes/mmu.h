#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct
{
    uint8_t ram[0x2000];
    //uint8_t ram[0x800];
    //uint8_t mirror_ram[0x800];

    uint8_t cart_ram[0x4000];
    uint8_t cart_mem[0x4000];
    uint8_t cart_mem_mirror[0x4000];
} mmu_t;


int mmu_init();
void mmu_exit();

int mmu_reset();

uint8_t mmu_read8(uint16_t addr);
uint16_t mmu_read16(uint16_t addr);
void mmu_write8(uint16_t addr, uint8_t v);
void mmu_write16(uint16_t addr, uint16_t v);

mmu_t *mem_get_mem();

#ifdef __cplusplus
}
#endif