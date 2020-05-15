#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cart.h"
#include "mapper.h"
#include "util.h"

static cart_t *cart = NULL;

#define HEADER_ID "NES"
#define HEADER_SIZE sizeof(rom_header_t)
#define ROM_SIZE_MAX _1MiB

void cart_eject();

const cart_t *cart_init()
{
    assert(cart == NULL);
    if (cart)
    {
        fprintf(stderr, "Cart already initialised\n");
        return NULL;
    }

    cart = malloc(sizeof(cart_t));
    assert(cart);
    if (!cart)
    {
        fprintf(stderr, "Failed to alloc cart\n");
        return NULL;
    }

    return cart;
}

void cart_exit()
{
    assert(cart);
    if (!cart)
    {
        fprintf(stderr, "Cart not initialised\n");
        return;
    }

    if (cart->loaded == true)
    {
        cart_eject();
    }

    free(cart);
    cart = NULL;
}

int cart_reset()
{
    /// currently only the mapper in the cart needs to be reset.
    /// I call this in the cart function because the mapper was also set in the cart function.
    /// so it makes sense to also reset it here.
    return mapper_reset();
}

void cart_eject()
{
    assert(cart);
    if (!cart)
    {
        fprintf(stderr, "Cart not initialised\n");
        return;
    }

    assert(cart->loaded);
    if (cart->loaded == true)
    {
        memset(&cart->header, 0, HEADER_SIZE);
        if (cart->rom)
        {
            free(cart->rom);
            cart->rom = NULL;
        }
        cart->size = 0;
        cart->loaded = false;
    }
}

int cart_load(const char *path)
{
    assert(cart);
    if (!cart)
    {
        fprintf(stderr, "Cart not initialised\n");
        return -1;
    }

    assert(path);
    if (!path)
    {
        fprintf(stderr, "Empty path in rom load\n");
        return -1;
    }

    /// if a previous rom was loaded, remove it.
    if (cart->loaded == true)
    {
        cart_eject();
    }

    FILE *fp = fopen(path, "rb");
    assert(fp);
    if (!fp)
    {
        fprintf(stderr, "Failed to open rom: %s\n", path);
        return -1;
    }

    /// get size.
    fseek(fp, 0, SEEK_END);
    uint32_t rom_size = ftell(fp);

    /// check is the size is at least the iNES header size.
    if (rom_size < HEADER_SIZE)
    {
        fprintf(stderr, "File size is smaller than rom header 0x%X %s\n", rom_size, path);
        goto fail_close;
    }

    /// check if the rom is no larger than 1MiB.
    if ((rom_size - HEADER_SIZE) > ROM_SIZE_MAX)
    {
        fprintf(stderr, "File size is larger than rom_size_max. GOT:0x%X MAX:0x%X ROM:%s\n", rom_size, ROM_SIZE_MAX, path);
        goto fail_close;
    }

    /// Don't forget to rewind after checking the size!
    rewind(fp);

    /// get header.
    rom_header_t header = {0};
    if (!fread(&header, HEADER_SIZE, 1, fp))
    {
        fprintf(stderr, "Failed to read rom header %s\n", path);
        goto fail_close;
    }

    /// check if the header is valid.
    bool is_ines = strncmp(header.id, HEADER_ID, 3) == 0;
    assert(is_ines);
    if (is_ines == false)
    {
        fprintf(stderr, "Not a iNES rom WANT: %s GOT:%s ROM:%s\n", HEADER_ID, header.id, path);
        goto fail_close;
    }

    /// TODO: parse header.
    printf("\n#### ROM-INFO ####\n");
    {
        printf("program rom size: %u\n", header.prg_rom_size * _16KB);
        printf("pattern rom size: %u\n", header.chr_rom_size * _8KB);
        printf("hw_nametable_type: %u\n", header.flags6.hw_nametable_type);
        printf("has battery: %s\n", bool_str(header.flags6.battery));
        printf("has trainer: %s\n", bool_str(header.flags6.trainer));
        printf("mapper number: %u\n", header.flags6.mapper_number);
    }
    printf("#### ROM-END ####\n\n");

    /// update rom size now that we have read the header.
    rom_size -= HEADER_SIZE;

    uint8_t *rom_data = malloc(rom_size);
    assert(rom_data);
    if (!rom_data)
    {
        fprintf(stderr, "Failed to alloc rom data %u %s\n", rom_size, path);
        goto fail_free;
    }

    /// load the rom data.
    if (!fread(rom_data, rom_size, 1, fp))
    {
        fprintf(stderr, "Failed read rom data %u %s\n", rom_size, path);
        goto fail_free;
    }

    fclose(fp);

    mapper_is_avaliable(header.flags6.mapper_number);
    
    memcpy(&cart->header, &header, HEADER_SIZE);
    cart->rom = rom_data;
    cart->size = rom_size;
    cart->loaded = true;

    return 0;

    fail_free:
    free(rom_data);

    fail_close:
    fclose(fp);
    return -1;
}

uint8_t cart_read(uint16_t addr)
{
    return cart->rom[addr];
}