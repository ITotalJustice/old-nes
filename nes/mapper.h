#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stdbool.h"

#include "cart.h"

typedef enum
{
    Mapper_0,

    Mapper_NONE = 0xFF,
} Mapper;

typedef uint8_t (*mapper_read_cb)(uint16_t addr);
typedef void (*mapper_write_cb)(uint16_t addr, uint8_t v);
typedef void (*mapper_dump_cb)(void);

typedef struct
{
    uint32_t pgr_rom_size;
    uint32_t chr_rom_size;

    mapper_read_cb read;
    mapper_write_cb write;
    mapper_dump_cb dump;
    Mapper type;
} mapper_t;

int mapper_init();
void mapper_exit();

int mapper_reset();

bool mapper_is_avaliable(Mapper mapper_type);
int mapper_set(const rom_header_t *header);

uint8_t mapper_read(uint16_t addr);
void mapper_write(uint16_t addr, uint8_t v);

#ifdef __cplusplus
}
#endif