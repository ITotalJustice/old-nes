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
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} apu_pulse_channel_t; // TODO: break these down into bits;

typedef struct
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} apu_triangle_channel_t;

typedef struct
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} apu_noise_channel_t;

typedef struct
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
} apu_dmc_channel_t;

typedef struct
{
    apu_pulse_channel_t pulse_1_channel;
    apu_pulse_channel_t pulse_2_channel;
    apu_triangle_channel_t triangle_channel;
    apu_noise_channel_t noise_channel;
    apu_dmc_channel_t dmc_channel;
    uint8_t status;
    uint8_t frame_counter;
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