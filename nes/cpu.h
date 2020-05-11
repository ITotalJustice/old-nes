#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

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
            uint8_t B:2; /// <no effect>
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
    uint32_t cycle;
    uint8_t opcode;

    bool halt;
} cpu_t;

void cpu_reset();
int cpu_init();
void cpu_exit();

#ifdef __cplusplus
}
#endif