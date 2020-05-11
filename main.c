#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ui/ui.hpp"

#include "nes/cpu.h"
#include "nes/mmu.h"
#include "nes/ppu.h"
#include "nes/apu.h"


int main(int argc, char **argv)
{
    printf("t-nes start\n");

    cpu_init();
    mmu_init();
    ppu_init();
    apu_init();

    ui();

    cpu_exit();
    mmu_exit();
    ppu_exit();
    apu_exit();
    return 0;
}