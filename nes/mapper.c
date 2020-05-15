#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mapper.h"
#include "cart.h"
#include "mappers/mapper_0.h"

/// NOTES:
/// So from what I understand on how mappers worked:
/// The cpu / ppu have a fixed address space and mem map. They don't actually know where stuff is on the cart. In fact, the address map name might
/// end up loading something completely different, it all depends on the rom.
/// The role of the mapper is to *map* the incomming address to the correct location.
/// Beacuse some games are quite large and have multiple rom banks etc, the mapper basically just intercepts everything.
/// I wasn't sure about implementing the mapper(s) this way as it would add function overhead, to something that will be called literally 50,000+ times a second.
/// But this is how the NES worked, and it just makes everything easier to follow the design first, optimise later.
/// Then again, it shouldn't be slow either.
/// Without ppu rendering, this code only uses 8-9% cpu with imgui and mem editor.

/// I also don't know a lot about the mappers yet, only mapper 0 which is dead simple.
/// I think rom bank switching works by if you write to xxxx address, it switches the banks.
/// This means that I would have to keep track of the bank.
/// Now saying that, I don't want to commit too much into writing a fancy mapper systems yet because maybe there's a mapper that has muliple banks,
/// and multiple prg roms and many chr roms also, i'd have to keep track of that.
/// Once I find out the actual limit to amount of diffrent stuff a rom can have, then it will be easy to keep track of.
/// Even as simple as having a global int which has the current rom bank loaded.

/// Lastly, this constant having to switch statement on every rw is a bad idea, even if it gets compiled down to a jump stable.
/// I have 2 other ways around this.
/// 1:
/// Have a struct that has 2 function callbacks, read and write.
/// When setting up the mapper, I'll have a function that will assign the functions to the struct.
/// 2:
/// C++ memes.

/// Naturally I like option 1 more.
/// I can even take it as far as to seperate mappers into sperate C files, all functions static, then just have a init function in each file.
/// The init function eill either map read / write functions OR map a basic function like mapper_access, then pass a enum with how i want to access the mapper.
/// Like maybe i want to read, write or dump stuff.
/// Probably overkill though, I could just assign the read write functions + the save / dump functions.

static mapper_t *mapper = NULL;

void mapper_unset();

int mapper_init()
{
    assert(mapper == NULL);
    if (mapper)
    {
        fprintf(stderr, "mapper already initialised\n");
        return -1;
    }

    mapper = malloc(sizeof(mapper_t));
    assert(mapper);
    if (!mapper)
    {
        fprintf(stderr, "Failed to alloc mapper\n");
        return -1;
    }

    mapper->read = NULL;
    mapper->write = NULL;
    mapper->dump = NULL;
    mapper->type = Mapper_NONE;

    return 0;
}

void mapper_exit()
{
    assert(mapper);
    if (!mapper)
    {
        fprintf(stderr, "mapper not initialised\n");
        return;
    }

    if (mapper->type != Mapper_NONE)
    {
        mapper_unset();
    }

    free(mapper);
    mapper = NULL;
}

int mapper_reset()
{
    assert(mapper);
    if (!mapper)
    {
        fprintf(stderr, "mapper not initialised\n");
        return -1;
    }

    assert(mapper->type != Mapper_NONE);
    if (mapper->type == Mapper_NONE)
    {
        fprintf(stderr, "mapper not set\n");
        return -1;
    }

    /// TODO:
    /// call mapper reset here.
    /// This will be needed for *some* mappers in order to reset rombanks (i think).

    return 0;
}

bool mapper_is_avaliable(Mapper mapper_type)
{
    switch (mapper_type)
    {
        case Mapper_0:  return true;
        default: return false;
    }
}

int mapper_set(const rom_header_t *header)
{
    assert(mapper);
    if (!mapper)
    {
        fprintf(stderr, "mapper not initialised\n");
        return -1;
    }

    assert(header);
    if (!header)
    {
        fprintf(stderr, "no rom header passed in mapper set\n");
        return -1;
    }

    if (mapper->type != Mapper_NONE)
    {
        mapper_unset();
    }

    switch (header->flags6.mapper_number)
    {
        case Mapper_0:  return mapper_0_init(mapper);

        default:
            fprintf(stderr, "mapper not yet supported\n");
            assert(0);
            return -1;
    }
}

void mapper_unset()
{
    assert(mapper);
    if (!mapper)
    {
        fprintf(stderr, "mapper not initialised\n");
        return;
    }

    assert(mapper->type != Mapper_NONE);
    if (mapper->type == Mapper_NONE)
    {
        fprintf(stderr, "mapper not set\n");
        return;
    }

    switch (mapper->type)
    {
        case Mapper_0:  mapper_0_exit();    break;

        default:
            fprintf(stderr, "mapper not yet supported\n");
            assert(0);
            break;
    }

    mapper->read = NULL;
    mapper->write = NULL;
    mapper->dump = NULL;
    mapper->type = Mapper_NONE;
}

uint8_t mapper_read(uint16_t addr)
{
    return mapper->read(addr);
}

void mapper_write(uint16_t addr, uint8_t v)
{
    mapper->write(addr, v);
}