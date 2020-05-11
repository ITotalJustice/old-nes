#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cpu.h"

static cpu_t *cpu = NULL;

void cpu_reset()
{
    cpu->reg.SP -= 3;
    cpu->reg.status_flag.I = true;
}

void cpu_power_up()
{
    cpu->reg.P = 0x34;
    cpu->reg.A = 0;
    cpu->reg.X = 0;
    cpu->reg.Y = 0;
    cpu->reg.SP = 0xFD;
}

int cpu_init()
{
    cpu = malloc(sizeof(cpu_t));
    assert(cpu);
    memset(cpu, 0, sizeof(cpu_t));

    return 0;
}

void cpu_exit()
{
    assert(cpu);
    free(cpu);
    cpu = NULL;
}

static inline void NIP()
{
    printf("NOT IMPLEMENTED: %u\n", cpu->opcode);
    assert(0);
}

void mem_write(uint16_t addr)
{

}

void mem_read(uint16_t addr)
{

}

void ADC(){}

void AND(){}

void ASL(){}

void BIT(){}

void BRK(){}

/*
*   Flag Instructions
*/

void CLC() /// clear carry
{
    cpu->reg.status_flag.C = false;
}

void CLI() /// clear interupt
{
    cpu->reg.status_flag.I = false;
}

void CLV() /// clear overflow
{
    cpu->reg.status_flag.V = false;
}

void CLD() /// clear deciaml
{
    cpu->reg.status_flag.D = false;
}

void SEC() /// set carry
{
    cpu->reg.status_flag.C = true;
}

void SEI() /// set interupt
{
    cpu->reg.status_flag.I = true;
}

void SED() // set decimal
{
    cpu->reg.status_flag.D = true;
}

void CMP(){}

void CPX(){}

void CPY(){}

void DEC(){}

void EOR(){}

void INC(){}

void JMP(){}

void JSR(){}

void LDA(){}

void LDX(){}

void LDY(){}

void LSR(){}

void NOP()
{
    cpu->cycle += 2;
}

void ORA(){}

void ROL(){}

void ROR(){}

void RTI(){}

void RTS(){}

void SBC(){}

void STA(){}

void STX(){}

void STY(){}


int execute(void)
{
    // 143 instructions.

    switch (cpu->opcode)
    {
        case 0x00:
            NIP();
            break;
        case 0x01:
            NIP();
            break;
        case 0x05:
            NIP();
            break;
        case 0x06:
            NIP();
            break;
        case 0x08:
            NIP();
            break;
        case 0x0A:
            NIP();
            break;
        case 0x0E:
            NIP();
            break;
        case 0x10:
            NIP();
            break;
        case 0x16:
            NIP();
            break;
        case 0x18:
            CLC();
            break;
        case 0x1E:
            NIP();
            break;
        case 0x20:
            NIP();
            break;
        case 0x21:
            NIP();
            break;
        case 0x24:
            NIP();
            break;
        case 0x25:
            NIP();
            break;
        case 0x26:
            NIP();
            break;
        case 0x28:
            NIP();
            break;
        case 0x2A:
            NIP();
            break;
        case 0x2C:
            NIP();
            break;
        case 0x2D:
            NIP();
            break;
        case 0x2E:
            NIP();
            break;
        case 0x30:
            NIP();
            break;
        case 0x31:
            NIP();
            break;
        case 0x35:
            NIP();
            break;
        case 0x36:
            NIP();
            break;
        case 0x38:
            SEC();
            break;
        case 0x39:
            NIP();
            break;
        case 0x3D:
            NIP();
            break;
        case 0x3E:
            NIP();
            break;
        case 0x40:
            NIP();
            break;
        case 0x41:
            NIP();
            break;
        case 0x45:
            NIP();
            break;
        case 0x46:
            NIP();
            break;
        case 0x48:
            NIP();
            break;
        case 0x49:
            NIP();
            break;
        case 0x4A:
            NIP();
            break;
        case 0x4C:
            NIP();
            break;
        case 0x4D:
            NIP();
            break;
        case 0x4E:
            NIP();
            break;
        case 0x50:
            NIP();
            break;
        case 0x51:
            NIP();
            break;
        case 0x55:
            NIP();
            break;
        case 0x56:
            NIP();
            break;
        case 0x58:
            CLI();
            break;
        case 0x59:
            NIP();
            break;
        case 0x5D:
            NIP();
            break;
        case 0x5E:
            NIP();
            break;
        case 0x60:
            NIP();
            break;
        case 0x61:
            NIP();
            break;
        case 0x65:
            NIP();
            break;
        case 0x66:
            NIP();
            break;
        case 0x68:
            NIP();
            break;
        case 0x69:
            NIP();
            break;
        case 0x6A:
            NIP();
            break;
        case 0x6C:
            NIP();
            break;
        case 0x6D:
            NIP();
            break;
        case 0x6E:
            NIP();
            break;
        case 0x70:
            NIP();
            break;
        case 0x71:
            NIP();
            break;
        case 0x75:
            NIP();
            break;
        case 0x76:
            NIP();
            break;
        case 0x78:
            SEI();
            break;
        case 0x79:
            NIP();
            break;
        case 0x7D:
            NIP();
            break;
        case 0x7E:
            NIP();
            break;
        case 0x81:
            NIP();
            break;
        case 0x84:
            NIP();
            break;
        case 0x85:
            NIP();
            break;
        case 0x86:
            NIP();
            break;
        case 0x88:
            NIP();
            break;
        case 0x8A:
            NIP();
            break;
        case 0x8C:
            NIP();
            break;
        case 0x8D:
            NIP();
            break;
        case 0x8E:
            NIP();
            break;
        case 0x90:
            NIP();
            break;
        case 0x91:
            NIP();
            break;
        case 0x94:
            NIP();
            break;
        case 0x95:
            NIP();
            break;
        case 0x96:
            NIP();
            break;
        case 0x98:
            NIP();
            break;
        case 0x99:
            NIP();
            break;
        case 0x9A:
            NIP();
            break;
        case 0xA0:
            NIP();
            break;
        case 0xA1:
            NIP();
            break;
        case 0xA2:
            NIP();
            break;
        case 0xA6:
            NIP();
            break;
        case 0xA8:
            NIP();
            break;
        case 0xA9:
            NIP();
            break;
        case 0xAA:
            NIP();
            break;
        case 0xAC:
            NIP();
            break;
        case 0xAD:
            NIP();
            break;
        case 0xAE:
            NIP();
            break;
        case 0xB0:
            NIP();
            break;
        case 0xB1:
            NIP();
            break;
        case 0xB4:
            NIP();
            break;
        case 0xB5:
            NIP();
            break;
        case 0xB6:
            NIP();
            break;
        case 0xB8:
            CLV();
            break;
        case 0xB9:
            NIP();
            break;
        case 0xBA:
            NIP();
            break;
        case 0xBC:
            NIP();
            break;
        case 0xBD:
            NIP();
            break;
        case 0xBE:
            NIP();
            break;
        case 0xC0:
            NIP();
            break;
        case 0xC1:
            NIP();
            break;
        case 0xC4:
            NIP();
            break;
        case 0xC5:
            NIP();
            break;
        case 0xC6:
            NIP();
            break;
        case 0xC8:
            NIP();
            break;
        case 0xC9:
            NIP();
            break;
        case 0xCA:
            NIP();
            break;
        case 0xD0:
            NIP();
            break;
        case 0xD1:
            NIP();
            break;
        case 0xD4:
            NIP();
            break;
        case 0xD5:
            NIP();
            break;
        case 0xD6:
            NIP();
            break;
        case 0xD8:
            CLD();
            break;
        case 0xD9:
            NIP();
            break;
        case 0xDD:
            NIP();
            break;
        case 0xDE:
            NIP();
            break;
        case 0xE0:
            NIP();
            break;
        case 0xE1:
            NIP();
            break;
        case 0xE4:
            NIP();
            break;
        case 0xE5:
            NIP();
            break;
        case 0xE6:
            NIP();
            break;
        case 0xE8:
            NIP();
            break;
        case 0xE9:
            NIP();
            break;
        case 0xEA:
            NIP();
            break;
        case 0xEC:
            NIP();
            break;
        case 0xED:
            NIP();
            break;
        case 0xEE:
            NIP();
            break;
        case 0xF0:
            NIP();
            break;
        case 0xF1:
            NIP();
            break;
        case 0xF5:
            NIP();
            break;
        case 0xF6:
            NIP();
            break;
        case 0xF8:
            SED();
            break;
        case 0xF9:
            NIP();
            break;
        case 0xFD:
            NIP();
            break;
        case 0xFE:
            NIP();
            break;
        default:
            NIP();
    }

    return 0;
}