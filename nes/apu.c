#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "apu.h"

static apu_t *apu = NULL;

void apu_reset()
{
    assert(apu);
    memset(apu, 0, sizeof(apu_t));
}

int apu_init()
{
    apu = malloc(sizeof(apu_t));
    assert(apu);
    apu_reset();

    return 0;
}

void apu_exit()
{
    assert(apu);
    free(apu);
    apu = NULL;
}