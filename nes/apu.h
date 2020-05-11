#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

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
} apu_triangle_channel_t;

typedef struct
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
} apu_noise_channel_t;

typedef struct
{
    uint8_t byte0;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;

    // TODO: are these part of the dmc channel?
    uint8_t byte4;
    uint8_t byte5;
    uint8_t byte6;
} apu_dmc_channel_t;

typedef struct
{
    apu_pulse_channel_t pulse_1_channel;
    apu_pulse_channel_t pulse_2_channel;
    apu_triangle_channel_t triangle_channel;
    apu_noise_channel_t noise_channel;
    apu_dmc_channel_t dmc_channel;
} apu_registers_t;

typedef struct
{
    apu_registers_t reg;
} apu_t;

void apu_reset();
int apu_init();
void apu_exit();

#ifdef __cplusplus
}
#endif