#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    PPURegisterAddr_PPUCTRL = 0x2000,
    PPURegisterAddr_PPUMASK,
    PPURegisterAddr_PPUSTATUS,
    PPURegisterAddr_OAMADDR,
    PPURegisterAddr_OAMDATA,
    PPURegisterAddr_PPUSCROLL,
    PPURegisterAddr_PPUADDR,
    PPURegisterAddr_PPUDATA,
    PPURegisterAddr_OAMDMA = 0x4014,
} PPURegisterAddr;

typedef struct
{
    union
    {
        struct
        {
            uint8_t base_table_addr:2;
            uint8_t vram_addr_inc:1;
            uint8_t sprite_pattern_table_addr:1;
            uint8_t bg_pattern_table_addr:1;
            uint8_t sprite_size:1;
            uint8_t ppu_select:1;
            uint8_t nmi:1;
        } ctrl;
        uint8_t ppu_ctrl;   // w
    };
    union
    {
        struct
        {
            uint8_t greyscale:1;
            uint8_t show_bg_leftmost:1;
            uint8_t show_sprites_leftmost:1;
            uint8_t show_gb:1;
            uint8_t show_sprites:1;
            uint8_t red:1;
            uint8_t green:1;
            uint8_t blue:1;
        } mask;
        uint8_t ppu_mask;   // w
    };

    union
    {
        struct
        {
            uint8_t _NA:5;
            uint8_t sprite_overflow:1;
            uint8_t sprite_0hit:1;
            uint8_t vblank:1;
        } status;
        uint8_t ppu_status; // r
    };

    uint8_t oam_addr;   // w
    uint8_t oam_data;   // rw
    uint8_t ppu_scroll; // w2
    uint8_t ppu_addr;   // w2
    uint8_t ppu_data;   // rw
    uint8_t oam_dma;    // w
} ppu_registers_t;

#define PATTERN_TABLE_PIXELS 16
typedef struct
{
    uint8_t pixel[PATTERN_TABLE_PIXELS];
} ppu_pattern_table_tile_t;

#define PATTERN_TABLE_TILES 256
typedef struct
{
    ppu_pattern_table_tile_t tiles[PATTERN_TABLE_TILES];
} ppu_pattern_table_t;

typedef struct
{
    uint8_t b[64];
} ppu_attribute_table_t;

#define NAMETABLE_ROWS 30 * 32
typedef struct
{
    uint8_t rows[NAMETABLE_ROWS];
    ppu_attribute_table_t attribute_table;
} ppu_nametable_t; /// 0x0400

#define PALETTE_COLOURS 3
typedef struct
{
    uint8_t colour[PALETTE_COLOURS]; /// each palette has 3 colours.
} ppu_palette_t;

typedef struct
{
    uint8_t background_colour;

    ppu_palette_t background_palette0;
    ppu_palette_t background_palette1;
    ppu_palette_t background_palette2;
    ppu_palette_t background_palette3;

    ppu_palette_t sprite_palette0;
    ppu_palette_t sprite_palette1;
    ppu_palette_t sprite_palette2;
    ppu_palette_t sprite_palette3;
} ppu_palettes_t;

typedef struct
{
    ppu_pattern_table_t pattern_table0; /// left
    ppu_pattern_table_t pattern_table1; /// right

    ppu_nametable_t nametable0;
    ppu_nametable_t nametable1;
    ppu_nametable_t nametable2;
    ppu_nametable_t nametable3;

    ppu_palettes_t palette_ram_indexes;
} ppu_memory_map_t;

typedef struct
{
    uint8_t sprite_y; /// y pos of top of the sprite.
    union
    {
        struct
        {
            uint8_t bank:1;
            uint8_t number:7; /// 0-254
        } _sprite_number;
        uint8_t sprite_number;
    };
    union
    {
        struct
        {
            uint8_t palette:2;
            uint8_t unimplemented:3;
            uint8_t priority:1; /// 0 = in front of bg. 1 = behind bg.
            uint8_t flip_horizontally:1;
            uint8_t flip_vertically:1;
        } _sprite_attribute;
        uint8_t sprite_attribute;
    };
    uint8_t sprite_x;
} ppu_oam_t;

typedef struct
{
    ppu_registers_t reg;
    ppu_memory_map_t mem;
    ppu_oam_t oam[64];
} ppu_t;

const ppu_t *ppu_init();
void ppu_exit();

int ppu_reset();

uint8_t ppu_read_register(uint16_t addr);
void ppu_write_register(uint16_t addr, uint8_t v);

int ppu_tick();

#ifdef __cplusplus
}
#endif