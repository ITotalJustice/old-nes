#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mmu.h"

static mmu_t *mmu = NULL;

void mmu_reset()
{
    assert(mmu);
    memset(mmu, 0, sizeof(mmu_t));
}

int mmu_init()
{
    mmu = malloc(sizeof(mmu_t));
    assert(mmu);
    mmu_reset();

    return 0;
}

void mmu_exit()
{
    assert(mmu);
    free(mmu);
    mmu = NULL;
}