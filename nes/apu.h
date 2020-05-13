#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
    APURegisterAddr_ST_Pulse1 = 0x4000,
    APURegisterAddr_ED_Pulse1 = 0x4003,

    APURegisterAddr_ST_Pulse2,
    APURegisterAddr_ED_Pulse2 = 0x4007,

    APURegisterAddr_ST_Triangle,
    APURegisterAddr_ED_Triangle = 0x400B,

    APURegisterAddr_ST_Noise,
    APURegisterAddr_ED_Noise = 0x400F,

    APURegisterAddr_ST_DMC,
    APURegisterAddr_ED_DMC = 0x4013,

    APURegisterAddr_Status = 0x4015,
    APURegisterAddr_FrameCounter = 0x4017,
} APURegisterAddr;

typedef struct
{
    union
    {
        struct
        {
            uint8_t vol:4;
            uint8_t constant:1;
            uint8_t loop:1;
            uint8_t duty:2;
        } _0;
        uint8_t byte0;
    };
    union
    {
        struct
        {
            uint8_t shift:3;
            uint8_t negate:1;
            uint8_t period:3;
            uint8_t enabled:1;
        } _1;
        uint8_t byte1;
    };
    union
    {
        struct
        {
            uint8_t timer_low;
        } _2;
        uint8_t byte2;
    };
    union
    {
        struct
        {
            uint8_t timer_high:3;
            uint8_t length_counter:5;
        } _3;
        uint8_t byte3;
    };
} apu_pulse_channel_t; // TODO: break these down into bits;

typedef struct
{
    union
    {
        struct
        {
            uint8_t linear_counter:7;
            uint8_t counter:1;
        } _0;
        uint8_t byte0;
    };
    union
    {
        struct
        {
            uint8_t unused;
        } _1;
        uint8_t byte1;
    };
    union
    {
        struct
        {
            uint8_t timer_low;
        } _2;
        uint8_t byte2;
    };
    union
    {
        struct
        {
            uint8_t timer_high:3;
            uint8_t length_counter:5;
        } _3;
        uint8_t byte3;
    };
} apu_triangle_channel_t;

typedef struct
{
    union
    {
        struct
        {
            uint8_t volume:4;
            uint8_t constant:1;
            uint8_t loop:1;
            uint8_t unused:2;
        } _0;
        uint8_t byte0;
    };
    union
    {
        struct
        {
            uint8_t unused;
        } _1;
        uint8_t byte1;
    };
    union
    {
        struct
        {
            uint8_t period:4;
            uint8_t unused:3;
            uint8_t loop:1;
        } _2;
        uint8_t byte2;
    };
    union
    {
        struct
        {
            uint8_t unused:3;
            uint8_t length_counter:5;
        } _3;
        uint8_t byte3;
    };
} apu_noise_channel_t;

typedef struct
{
    union
    {
        struct
        {
            uint8_t frequency:4;
            uint8_t unused:2;
            uint8_t loop:1;
            uint8_t irq:1;
        } _0;
        uint8_t byte0;
    };
    union
    {
        struct
        {
            uint8_t unused:1;
            uint8_t load_counter:7;
        } _1;
        uint8_t byte1;
    };
    union
    {
        struct
        {
            uint8_t sample_addr;
        } _2;
        uint8_t byte2;
    };
    union
    {
        struct
        {
            uint8_t sample_length;
        } _3;
        uint8_t byte3;
    };
} apu_dmc_channel_t;

typedef struct
{
    apu_pulse_channel_t pulse_1_channel;
    apu_pulse_channel_t pulse_2_channel;
    apu_triangle_channel_t triangle_channel;
    apu_noise_channel_t noise_channel;
    apu_dmc_channel_t dmc_channel;

    union
    {
        struct
        {
            uint8_t pulse1:1;
            uint8_t pulse2:1;
            uint8_t triangle:1;
            uint8_t noise:1;
            uint8_t dmc:1;
            uint8_t unused:3;
        } _status_write;
        struct
        {
            uint8_t length_counter:4;
            uint8_t dmc:1;
            uint8_t unused:1;
            uint8_t interrupt_frame:1;
            uint8_t interrupt_dmc:1;
        } _status_read;
        uint8_t status;
    };
    union
    {
        struct
        {
            uint8_t unused:6;
            uint8_t irq:1;
            uint8_t mode:1;
        } _frame_counter;
        uint8_t frame_counter;
    };
} apu_registers_t;

typedef struct
{
    apu_registers_t reg;
} apu_t;

void apu_reset();
int apu_init();
void apu_exit();

uint8_t apu_read_register(uint16_t addr);
void apu_write_register(uint16_t addr, uint8_t v);

#ifdef __cplusplus
}
#endif