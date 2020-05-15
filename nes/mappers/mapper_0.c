#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../mapper.h"

static uint8_t read(uint16_t addr);
static void write(uint16_t addr, uint8_t v);
static void dump();

int mapper_0_init(mapper_t *mapper)
{
    mapper->read = read;
    mapper->write = write;
    mapper->dump = dump;
    mapper->type = Mapper_0;

    return 0;
}

void mapper_0_exit()
{

}

static uint8_t read(uint16_t addr)
{
    return 0;
}

static void write(uint16_t addr, uint8_t v)
{

}

static void dump()
{

}