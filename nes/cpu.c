#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cpu.h"


cpu_t *cpu = NULL;

inline void tick(uint8_t c)
{
    assert(c > 0);
    cpu->cycle += c;
}

uint8_t read8(uint16_t addr)
{
    tick(1);
    return 0;
}

uint16_t read16(uint16_t addr)
{
    tick(2);
    return 0;
}

void write8(uint16_t addr, uint8_t v)
{
    tick(1);
}

void write16(uint16_t addr, uint16_t v)
{
    tick(2);
}

/// https://youtu.be/fWqBmmPQP40?t=556
void address_load(AddressingType type, bool page_cross)
{
    switch (type)
    {
        case AddressingType_Immediate:
            cpu->oprand = cpu->reg.PC++;
            break;
        
        case AddressingType_Absolute:
            cpu->oprand = read16(cpu->reg.PC);
            cpu->reg.PC += 2;
            break;
        
        case AddressingType_AbsoluteX:
            if (page_cross && (cpu->reg.PC & 0x0F00) != ((cpu->reg.PC + cpu->reg.X) & 0x0F00))
            {
                tick(1);
            }

            cpu->oprand = read16(cpu->reg.PC + cpu->reg.X);
            cpu->reg.PC += 2;
            break;

        case AddressingType_AbsoluteY:
            if (page_cross && (cpu->reg.PC & 0x0F00) != ((cpu->reg.PC + cpu->reg.Y) & 0x0F00))
            {
                tick(1);
            }

            cpu->oprand = read16(cpu->reg.PC + cpu->reg.Y);
            cpu->reg.PC += 2;
            break;

        case AddressingType_ZeroPage:
            cpu->oprand = read8(cpu->reg.PC++);
            break;
        
        case AddressingType_ZeroPageX:
        {
            uint16_t addr = cpu->reg.PC + cpu->reg.X;
            if (addr > 0xFF) addr -= 0xFF;
            cpu->oprand = read8(addr);
            ++cpu->reg.PC;
            break;
        }

        case AddressingType_ZeroPageY:
        {
            uint16_t addr = cpu->reg.PC + cpu->reg.Y;
            if (addr > 0xFF) addr -= 0xFF;
            cpu->oprand = read8(addr);
            ++cpu->reg.PC;
            break;
        }

        case AddressingType_IndirectZeroPageX:
        {
            uint16_t addr = cpu->reg.PC + cpu->reg.X;
            if (addr > 0xFF) addr -= 0xFF;
            cpu->oprand = read8(read16(addr));
            cpu->reg.PC += 2;
            break;
        }

        // TODO: check if PC can be > 0xFF.
        // Different to indirectX in that it does the lookup first, then adds Y.
        case AddressingType_IndirectZeroPageY:
        {
            cpu->oprand = read8(read16(cpu->reg.PC) + cpu->reg.Y);
            cpu->reg.PC += 2;
            break;
        }
        default:
            printf("INCORRECT ADDRESS TYPE: %u", type);
            assert(0);
    }
}

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


/*
*   ALU. 
*/
void ADC()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void AND()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void ASL()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void BIT(){}

void BRK()
{
    cpu->reg.status_flag.B = true;
}


/*
*   Flag Instructions
*/
void CLC() /// clear carry
{
    cpu->reg.status_flag.C = false;
    tick(2);
}

void CLI() /// clear interupt
{
    cpu->reg.status_flag.I = false;
    tick(2);
}

void CLV() /// clear overflow
{
    cpu->reg.status_flag.V = false;
    tick(2);
}

void CLD() /// clear deciaml
{
    cpu->reg.status_flag.D = false;
    tick(2);
}

void SEC() /// set carry
{
    cpu->reg.status_flag.C = true;
    tick(2);
}

void SEI() /// set interupt
{
    cpu->reg.status_flag.I = true;
    tick(2);
}

void SED() // set decimal
{
    cpu->reg.status_flag.D = true;
    tick(2);
}


void CMP()
{
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void CPX()
{
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void CPY()
{
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void DEC()
{
    /*
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
    */
}

void EOR()
{
    /*
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
    */
}

void INC()
{
    /*
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
    */
}

void JMP(){}

void JSR(){}

void LDA()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void LDX()
{
    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1;
}

void LDY()
{
    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = (cpu->reg.Y >> 7) == 1;
}

void LSR()
{
    /*
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
    */
}

void NOP()
{
    tick(2);
}

void ORA()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void PLA()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

/*
*   Register Instructions.
*/
void TAX()
{
    cpu->reg.X = cpu->reg.A;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1; 

    tick(2);
}

void TXA()
{
    cpu->reg.A = cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 

    tick(2);
}

void TSX()
{
    cpu->reg.X = cpu->reg.SP;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1; 

    tick(2);
}

void TXS()
{
    cpu->reg.SP = cpu->reg.X;

    tick(2);
}

void DEX()
{
    --cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1; 

    tick(2);
}

void INX()
{
    ++cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1; 

    tick(2);
}

void TAY()
{
    cpu->reg.Y = cpu->reg.A;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = (cpu->reg.Y >> 7) == 1; 

    tick(2);
}

void TYA()
{
    cpu->reg.A = cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 

    tick(2);
}

void DEY()
{
    --cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = (cpu->reg.Y >> 7) == 1; 

    tick(2);
}

void INY()
{
    ++cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = (cpu->reg.Y >> 7) == 1; 

    tick(2);
}


void ROL()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void ROR()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    //cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void RTI(){}

void RTS(){}

void SBC()
{
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;    
}

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
            DEY();
            break;
        case 0x8A:
            TXA();
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
            TYA();
            break;
        case 0x99:
            NIP();
            break;
        case 0x9A:
            TXS();
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
            TAY();
            break;
        case 0xA9:
            NIP();
            break;
        case 0xAA:
            TAX();
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
            TSX();
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
            INY();
            break;
        case 0xC9:
            NIP();
            break;
        case 0xCA:
            DEX();
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
            INX();
            break;
        case 0xE9:
            NIP();
            break;
        case 0xEA:
            NOP();
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