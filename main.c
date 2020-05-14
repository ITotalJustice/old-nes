#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ui/ui.hpp"

#include "nes/nes.h"


int main(int argc, char **argv)
{
    printf("t-nes start\n");

    nes_init();

    if (argc > 1)
    {
        nes_loadrom(argv[1]);
    }

    ui();

    nes_exit();
    
    return 0;
}