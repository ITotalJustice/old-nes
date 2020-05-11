#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ppu.h"

static ppu_t *ppu = NULL;

void ppu_reset()
{
    assert(ppu);
    memset(ppu, 0, sizeof(ppu_t));
}

int ppu_init()
{
    ppu = malloc(sizeof(ppu_t));
    assert(ppu);
    ppu_reset();

    return 0;
}

void ppu_exit()
{
    assert(ppu);
    free(ppu);
    ppu = NULL;
}