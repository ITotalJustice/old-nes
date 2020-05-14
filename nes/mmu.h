#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define INTERNAL_RAM_SIZE 0x2000
typedef struct
{
    uint8_t ram[INTERNAL_RAM_SIZE];
} mmu_t;


const mmu_t *mmu_init();
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