#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    CPUMemMap_ST_Ram,
    CPUMemMap_ED_Ram = 0x07FF,
    CPUMemMap_ST_RamMirror,
    CPUMemMap_ED_RamMirror = 0x1FFF,
    CPUMemMap_ST_PPUReg,
    CPUMemMap_ED_PPUReg = 0x2007,
    CPUMemMap_ST_PPURegMirror,
    CPUMemMap_ED_PPURegMirror = 0x3FFF,
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
typedef enum
{
    AddrType_Acc,
    AddrType_Imp,
    AddrType_Rel,
    AddrType_Imm,
    AddrType_Abs,
    AddrType_AbsX,
    AddrType_AbsY,
    AddrType_ZP,
    AddrType_ZPX,
    AddrType_ZPY,
    AddrType_Ind,
    AddrType_IndZPX,
    AddrType_IndZPY,
} AddrType;

typedef struct
{
    union
    {
        struct
        {
            uint8_t C:1; /// carry
            uint8_t Z:1; /// zero
            uint8_t I:1; /// interrupt disable
            uint8_t D:1; /// decimal
            uint8_t B:1; /// <no effect>
            uint8_t U:1; /// unkown.
            uint8_t V:1; /// overflow
            uint8_t N:1; /// negative
        } status_flag;
        uint8_t P;
    };
    uint8_t A;
    uint8_t X;
    uint8_t Y;

    uint8_t SP;
    uint16_t PC;
} cpu_register_t;

typedef struct
{
    cpu_register_t reg;

    uint8_t internal_ram[2048];

    uint32_t cycle;
    uint64_t cycle_total;

    union
    {
        struct
        {
            uint8_t low;
            uint8_t high;
        } oprand_byte;
        uint16_t oprand;
    };
    uint8_t opcode;

    struct
    {
        uint64_t count;
    } debug;
} cpu_t;


const cpu_t *cpu_init();
void cpu_exit();

int cpu_power_up();
int cpu_reset();

void cpu_reset_cycle();

int cpu_tick();

/// debug
cpu_t *cpu_debug_get();

#ifdef __cplusplus
}
#endif