#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    AddressingType_Relative,
    AddressingType_Immediate,
    AddressingType_Absolute,
    AddressingType_AbsoluteX,
    AddressingType_AbsoluteY,
    AddressingType_ZeroPage,
    AddressingType_ZeroPageX,
    AddressingType_ZeroPageY,
    AddressingType_IndirectZeroPageX,
    AddressingType_IndirectZeroPageY,
} AddressingType;

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

    uint16_t oprand; /// can be 1 or 2 bytes depending on the addressing.
    uint8_t opcode;
} cpu_t;

void cpu_power_up();
void cpu_reset();
int cpu_init();
void cpu_exit();


/// debug
void cpu_debug_step();
cpu_t *cpu_debug_get();

#ifdef __cplusplus
}
#endif