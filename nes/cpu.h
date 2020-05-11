#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint16_t PC;
    uint8_t SP;
    union
    {
        struct
        {
            uint8_t C:1;
            uint8_t Z:1;
            uint8_t I:1;
            uint8_t D:1;
            uint8_t B:1;
            uint8_t U:1;
            uint8_t V:1;
            uint8_t N:1;
        } status_flag;
        uint8_t status;
    };
    uint8_t A;
    uint8_t X;
    uint8_t Y;
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