#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HeaderType_None,
    HeaderType_ArchiNES,
    HeaderType_iNES,
    HeaderType_NES2,
} HeaderType;

typedef enum
{
    ConsoleType_NES_Famicon,
    ConsoleType_Vs_System,
    ConsoleType_Playchoice_10,
    ConsoleType_Extended_console,
} ConsoleType;

typedef enum
{
    CPU_PPU_TimingMode_RP2C02, // NTSC
    CPU_PPU_TimingMode_RP2C07, // PAL
    CPU_PPU_TimingMode_Multiple_Region,
    CPU_PPU_TimingMode_UMC6527P,
} CPU_PPU_TimingMode;

typedef enum
{
    Vs_System_PPU_Type_RP2C03B,
    Vs_System_PPU_Type_RP2C03G,
    Vs_System_PPU_Type_RP2C04_0001,
    Vs_System_PPU_Type_RP2C04_0002,
    Vs_System_PPU_Type_RP2C04_0003,
    Vs_System_PPU_Type_RP2C04_0004,
    Vs_System_PPU_Type_RC2C03B,
    Vs_System_PPU_Type_RC2C03C,
    Vs_System_PPU_Type_RP2C05_01,
    Vs_System_PPU_Type_RP2C05_02,
    Vs_System_PPU_Type_RP2C05_03,
    Vs_System_PPU_Type_RP2C05_04,
    Vs_System_PPU_Type_RP2C05_05,
} Vs_System_PPU_Type;

typedef enum
{
    Vs_System_Hardware_Type_Unisys_Normal,
    Vs_System_Hardware_Type_Unisys_RBIBaseball,
    Vs_System_Hardware_Type_Unisys_TKOBoxing,
    Vs_System_Hardware_Type_Unisys_SuperXevious,
    Vs_System_Hardware_Type_Unisys_IceClimber,
} Vs_System_Hardware_Type;

typedef enum
{
    ExtendedConsoleType_Normal,
    ExtendedConsoleType_Vs_System,
    ExtendedConsoleType_Playchoice_10,
    ExtendedConsoleType_Famiclone,
    ExtendedConsoleType_VT01_Monochrome,
    ExtendedConsoleType_VT01_Red_Cyan,
    ExtendedConsoleType_VT0w,
    ExtendedConsoleType_VT03,
    ExtendedConsoleType_VT09,
    ExtendedConsoleType_VT32,
    ExtendedConsoleType_VT369,
    ExtendedConsoleType_UMC_UM6578,
} ExtendedConsoleType;

typedef struct
{
    char id[0x4];
    uint8_t prg_rom_size; /// *16kb
    uint8_t chr_rom_size; /// *8kb
    
    struct
    {
        uint8_t hw_nametable_type:1;
        uint8_t battery:1;
        uint8_t trainer:1;
        uint8_t hw_four_screen_mode:1;
        uint8_t mapper_number:4;
    } flags6;

    struct
    {
        uint8_t console_type:2;
        uint8_t nes2_0:2;
        uint8_t mapper_number:4;
    } flags7;

    union
    {
        struct
        {
            uint8_t pgr_ram_size;
        } flags8;

        struct
        {
            uint8_t mapper_number:4;
            uint8_t submapper_number:4;
        } mapper_submapper;
    };

    union
    {
        struct
        {
            uint8_t tv_system:1;
            uint8_t reserved:7;
        } flags9;

        struct
        {
            uint8_t pgr:4;
            uint8_t chr:4;
        } prg_rom_chr_rom_size;
    };

    union
    {
        struct
        {
            uint8_t tv_system:2;
            uint8_t unused0:2;
            uint8_t pgrm_ram:1;
            uint8_t bus_conflicts:1;
            uint8_t unused1:2;
        } flags10;

        struct
        {
            uint8_t pgr:4;
            uint8_t eeprom:4;
        } pgr_ram_eeprom_size;
    };

    union
    {
        struct
        {
            uint8_t unused;
        } flags11;

        struct
        {
            uint8_t ram:4;
            uint8_t nvram:4;
        } chr_ram_size;
    };

    union
    {
        struct
        {
            uint8_t unused;
        } flags12;

        struct
        {
            uint8_t mode:2;
            uint8_t _NA:6;
        } cpu_ppu_timing;
    };

    union
    {
        struct
        {
            uint8_t unused;
        } flags13;

        struct // if Vs. System.
        {
            uint8_t ppu_type:4;
            uint8_t hardware_type:4;
        } vs_system_type;

        struct
        {
            uint8_t type:4;
            uint8_t _NA:4;
        } extended_console_type;
    };

    union
    {
        struct
        {
            uint8_t unused;
        } flags14;

        struct
        {
            uint8_t count:2;
            uint8_t _NA:6;
        } misc_roms;
    };

    union
    {
        struct
        {
            uint8_t unused;
        } flags15;

        struct
        {
            uint8_t device:6;
            uint8_t _NA:2;
        } default_expansion_device;
    };
} rom_header_t;

typedef struct
{
    uint8_t data[512];
} trainer_area_t;

typedef struct
{
    rom_header_t header;
    HeaderType header_type;

    bool loaded;
    uint8_t *rom;
    uint32_t size;
} cart_t;


const cart_t *cart_init();
void cart_exit();

void cart_eject();
int cart_load(const char *path);

uint8_t cart_read(uint16_t addr);

#ifdef __cplusplus
}
#endif