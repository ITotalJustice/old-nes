#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "apu.h"

static apu_t *apu = NULL;

const apu_t *apu_init()
{
    assert(apu == NULL);
    if (apu)
    {
        fprintf(stderr, "apu already initialised\n");
        return NULL;
    }

    apu = malloc(sizeof(apu_t));
    assert(apu);
    if (!apu)
    {
        fprintf(stderr, "Failed to alloc apu\n");
        return NULL;
    }
    
    return apu;
}

void apu_exit()
{
    assert(apu);
    if (!apu)
    {
        fprintf(stderr, "apu not initialised\n");
        return;
    }

    free(apu);
    apu = NULL;
}

int apu_reset()
{
    assert(apu);
    if (!apu)
    {
        fprintf(stderr, "apu not initialised\n");
        return -1;
    }

    memset(apu, 0, sizeof(apu_t));

    return 0;
}

uint8_t apu_read_register(uint16_t addr)
{
    switch (addr)
    {
        case APURegisterAddr_ST_Pulse1 ... APURegisterAddr_ED_Pulse1:
            switch (addr)
            {
                case APURegisterAddr_ST_Pulse1 + 0: return apu->reg.pulse_1_channel.byte0;
                case APURegisterAddr_ST_Pulse1 + 1: return apu->reg.pulse_1_channel.byte1;
                case APURegisterAddr_ST_Pulse1 + 2: return apu->reg.pulse_1_channel.byte2;
                case APURegisterAddr_ST_Pulse1 + 3: return apu->reg.pulse_1_channel.byte3;
            }
        case APURegisterAddr_ST_Pulse2 ... APURegisterAddr_ED_Pulse2:
            switch (addr)
            {
                case APURegisterAddr_ST_Pulse2 + 0: return apu->reg.pulse_1_channel.byte0;
                case APURegisterAddr_ST_Pulse2 + 1: return apu->reg.pulse_1_channel.byte1;
                case APURegisterAddr_ST_Pulse2 + 2: return apu->reg.pulse_1_channel.byte2;
                case APURegisterAddr_ST_Pulse2 + 3: return apu->reg.pulse_1_channel.byte3;
            }
        case APURegisterAddr_ST_Triangle ... APURegisterAddr_ED_Triangle:
            switch (addr)
            {
                case APURegisterAddr_ST_Triangle + 0: return apu->reg.triangle_channel.byte0;
                case APURegisterAddr_ST_Triangle + 1: return apu->reg.triangle_channel.byte1;
                case APURegisterAddr_ST_Triangle + 2: return apu->reg.triangle_channel.byte2;
                case APURegisterAddr_ST_Triangle + 3: return apu->reg.triangle_channel.byte3;
            }
        case APURegisterAddr_ST_Noise ... APURegisterAddr_ED_Noise:
            switch (addr)
            {
                case APURegisterAddr_ST_Noise + 0: return apu->reg.triangle_channel.byte0;
                case APURegisterAddr_ST_Noise + 1: return apu->reg.triangle_channel.byte1;
                case APURegisterAddr_ST_Noise + 2: return apu->reg.triangle_channel.byte2;
                case APURegisterAddr_ST_Noise + 3: return apu->reg.triangle_channel.byte3;
            }
        case APURegisterAddr_ST_DMC ... APURegisterAddr_ED_DMC:
            switch (addr)
            {
                case APURegisterAddr_ST_DMC + 0: return apu->reg.triangle_channel.byte0;
                case APURegisterAddr_ST_DMC + 1: return apu->reg.triangle_channel.byte1;
                case APURegisterAddr_ST_DMC + 2: return apu->reg.triangle_channel.byte2;
                case APURegisterAddr_ST_DMC + 3: return apu->reg.triangle_channel.byte3;
            }
        case APURegisterAddr_Status: return apu->reg.status;
        case APURegisterAddr_FrameCounter: return apu->reg.frame_counter;
        default:
            fprintf(stderr, "READING FROM NON VALID ADDRESS IN APU READ REG: 0x%04X\n", addr);
            assert(0);
            return 0;
    }
}

void apu_write_register(uint16_t addr, uint8_t v)
{
    switch (addr)
    {
        case APURegisterAddr_ST_Pulse1 ... APURegisterAddr_ED_Pulse1:
            switch (addr)
            {
                case APURegisterAddr_ST_Pulse1 + 0: apu->reg.pulse_1_channel.byte0 = v; break;
                case APURegisterAddr_ST_Pulse1 + 1: apu->reg.pulse_1_channel.byte1 = v; break;
                case APURegisterAddr_ST_Pulse1 + 2: apu->reg.pulse_1_channel.byte2 = v; break;
                case APURegisterAddr_ST_Pulse1 + 3: apu->reg.pulse_1_channel.byte3 = v; break;
            }
        case APURegisterAddr_ST_Pulse2 ... APURegisterAddr_ED_Pulse2:
            switch (addr)
            {
                case APURegisterAddr_ST_Pulse2 + 0: apu->reg.pulse_1_channel.byte0 = v; break;
                case APURegisterAddr_ST_Pulse2 + 1: apu->reg.pulse_1_channel.byte1 = v; break;
                case APURegisterAddr_ST_Pulse2 + 2: apu->reg.pulse_1_channel.byte2 = v; break;
                case APURegisterAddr_ST_Pulse2 + 3: apu->reg.pulse_1_channel.byte3 = v; break;
            }
        case APURegisterAddr_ST_Triangle ... APURegisterAddr_ED_Triangle:
            switch (addr)
            {
                case APURegisterAddr_ST_Triangle + 0: apu->reg.triangle_channel.byte0 = v; break;
                case APURegisterAddr_ST_Triangle + 1: apu->reg.triangle_channel.byte1 = v; break;
                case APURegisterAddr_ST_Triangle + 2: apu->reg.triangle_channel.byte2 = v; break;
                case APURegisterAddr_ST_Triangle + 3: apu->reg.triangle_channel.byte3 = v; break;
            }
        case APURegisterAddr_ST_Noise ... APURegisterAddr_ED_Noise:
            switch (addr)
            {
                case APURegisterAddr_ST_Noise + 0: apu->reg.triangle_channel.byte0 = v; break;
                case APURegisterAddr_ST_Noise + 1: apu->reg.triangle_channel.byte1 = v; break;
                case APURegisterAddr_ST_Noise + 2: apu->reg.triangle_channel.byte2 = v; break;
                case APURegisterAddr_ST_Noise + 3: apu->reg.triangle_channel.byte3 = v; break;
            }
        case APURegisterAddr_ST_DMC ... APURegisterAddr_ED_DMC:
            switch (addr)
            {
                case APURegisterAddr_ST_DMC + 0: apu->reg.triangle_channel.byte0 = v; break;
                case APURegisterAddr_ST_DMC + 1: apu->reg.triangle_channel.byte1 = v; break;
                case APURegisterAddr_ST_DMC + 2: apu->reg.triangle_channel.byte2 = v; break;
                case APURegisterAddr_ST_DMC + 3: apu->reg.triangle_channel.byte3 = v; break;
            }
        case APURegisterAddr_Status: apu->reg.status = v; break;
        case APURegisterAddr_FrameCounter: apu->reg.frame_counter = v; break;
        default:
            fprintf(stderr, "READING FROM NON VALID ADDRESS IN APU READ REG: 0x%04X\n", addr);
            assert(0);
            break;
    }
}

int apu_tick()
{
    return 0;
}