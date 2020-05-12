#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cpu.h"
#include "mmu.h"

#define BIT0 (1)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)


cpu_t *cpu = NULL;

inline void tick(uint8_t c)
{
    assert(c > 0);
    cpu->cycle += c;
}

uint8_t read8(uint16_t addr)
{
    tick(1);
    return mmu_read8(addr);
}

uint16_t read16(uint16_t addr)
{
    /// read16 along with write16 isn't actually a nes thing.
    /// the nes would read a byte, inc the pc, then read another byte, then inc the pc.
    /// same for the write16(?).
    /// however, i cannot think of a single reason for this to effect accuracy.
    /// so hence read/write 2 bytes directly for performance.
    tick(2);
    return mmu_read16(addr);
}

void write8(uint16_t addr, uint8_t v)
{
    mmu_write8(addr, v);
    tick(1);
}

void write16(uint16_t addr, uint16_t v)
{
    mmu_write16(addr, v);
    tick(2);
}

/// https://youtu.be/fWqBmmPQP40?t=556
/// http://archive.6502.org/books/mcs6500_family_hardware_manual.pdf
void address_load(AddressingType type)
{
    switch (type)
    {
        case AddressingType_Relative:
            cpu->oprand = cpu->reg.PC++;
            tick(1);
            break;

        case AddressingType_Immediate:
            cpu->oprand = cpu->reg.PC++;
            tick(1);
            break;
        
        case AddressingType_Absolute:
            cpu->oprand = read16(cpu->reg.PC);
            cpu->reg.PC += 2;
            break;
        
        case AddressingType_AbsoluteX:
            if ((cpu->reg.PC & 0x0F00) != ((cpu->reg.PC + cpu->reg.X) & 0x0F00))
            {
                tick(1);
            }

            cpu->oprand = read16(cpu->reg.PC + cpu->reg.X);
            cpu->reg.PC += 2;
            break;

        case AddressingType_AbsoluteY:
            if ((cpu->reg.PC & 0x0F00) != ((cpu->reg.PC + cpu->reg.Y) & 0x0F00))
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

    /// TODO: REMOVE THIS
    cpu->reg.PC = 0xC000;
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
    printf("NOT IMPLEMENTED opcode: 0x%X PC: 0x%X\n", cpu->opcode, cpu->reg.PC);
    assert(0);
}


/*
*   ALU. 
*/
void ADC()
{
    uint8_t old_carry_flag = cpu->reg.status_flag.C;
    uint8_t old_a_value = cpu->reg.A;
    uint8_t v = read8(cpu->oprand);
    cpu->reg.status_flag.C = cpu->reg.A + v + cpu->reg.status_flag.C > 0xFF;
    cpu->reg.A += v + old_carry_flag;

    cpu->reg.status_flag.Z = cpu->reg.A == 0;
    cpu->reg.status_flag.V = !(((old_a_value ^ cpu->reg.A) & 0x80) && (old_a_value ^ v) & 0x80);
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void AND()
{
    cpu->reg.A &= cpu->oprand;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void ASL_A()
{
    cpu->reg.status_flag.C = cpu->reg.A >> 7;

    cpu->reg.A <<= 1;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void ASL()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v << 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = v >> 7;
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void SBC()
{
    uint8_t old_carry_flag = cpu->reg.status_flag.C;
    uint8_t old_a_value = cpu->reg.A;
    uint8_t v = read8(cpu->oprand);
    cpu->reg.status_flag.C = cpu->reg.A < (v + cpu->reg.status_flag.C);
    cpu->reg.A -= v + old_carry_flag;

    cpu->reg.status_flag.Z = cpu->reg.A == 0;
    cpu->reg.status_flag.V = (((old_a_value ^ cpu->reg.A) & 0x80) && (old_a_value ^ v) & 0x80);
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1; 
}

void BIT()
{
    uint8_t v = read8(cpu->oprand);

    cpu->reg.status_flag.Z = (cpu->reg.A & v) == 1;
    cpu->reg.status_flag.V = (v & 0x40) >> 6;
    cpu->reg.status_flag.N = v >> 7;
}

void BRK()
{
    cpu->reg.status_flag.B = true;
}


/*
*   Branch Time!
*/
/// TODO: correct tick()
/// all branch are 2 cycles, + 1 if taken, + 1 if page cross.
void BCC()
{
    if (cpu->reg.status_flag.C == 0)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BCS()
{
    if (cpu->reg.status_flag.C == 1)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BEQ()
{
    if (cpu->reg.status_flag.Z == 1)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BNE()
{
    if (cpu->reg.status_flag.Z == 0)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BMI()
{
    if (cpu->reg.status_flag.N == 1)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BPL()
{
    if (cpu->reg.status_flag.N == 0)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BVS()
{
    if (cpu->reg.status_flag.V == 1)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
}

void BVC()
{
    if (cpu->reg.status_flag.V == 0)
    {
        cpu->reg.PC += (int8_t)read8(cpu->oprand);
    }
    tick(2);
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
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.A - v;

    cpu->reg.status_flag.C = (cpu->reg.A >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = (r >> 7) == 1; 
}

void CPX()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.X - v;

    cpu->reg.status_flag.C = (cpu->reg.X >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = (r >> 7) == 1; 
}

void CPY()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.Y - v;

    cpu->reg.status_flag.C = (cpu->reg.Y >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = (r >> 7) == 1; 
}

void DEC()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v - 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void EOR()
{
    cpu->reg.A ^= read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void INC()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v + 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void JMP()
{
    cpu->reg.PC = cpu->oprand;
}

void JSR()
{
    write16(cpu->reg.SP + 0x100, cpu->reg.PC - 1);
    cpu->reg.SP -= 2;
    cpu->reg.PC = cpu->oprand;
}

void LDA()
{
    cpu->reg.A = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void LDX()
{
    cpu->reg.X = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = (cpu->reg.X >> 7) == 1;
}

void LDY()
{
    cpu->reg.Y = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = (cpu->reg.Y >> 7) == 1;
}

void LSR_A()
{
    cpu->reg.status_flag.C = cpu->reg.A & 1;

    cpu->reg.A >>= 1;
    
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void LSR()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v >> 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = v & 1;
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void NOP()
{
    tick(2);
}

void ORA()
{
    cpu->reg.A |= read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void PHA()
{
    write8(cpu->reg.SP + 0x100, cpu->reg.A);
    --cpu->reg.SP;
}

void PHP()
{
    write8(cpu->reg.SP + 0x100, cpu->reg.P);
    --cpu->reg.SP;;
}

void PLA()
{
    ++cpu->reg.SP;
    cpu->reg.A = read8(cpu->reg.SP + 0x100);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void PLP()
{
    ++cpu->reg.SP;
    cpu->reg.P = read8(cpu->reg.SP + 0x100);
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

void ROL_A()
{
    uint8_t old_c_flag = cpu->reg.status_flag.C;
    cpu->reg.status_flag.C = cpu->reg.A >> 7;

    cpu->reg.A = (cpu->reg.A << 1) | old_c_flag;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void ROL()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = (v << 1) | cpu->reg.status_flag.C;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = (v >> 7);
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void ROR_A()
{
    uint8_t old_c_flag = cpu->reg.status_flag.C;
    cpu->reg.status_flag.C = cpu->reg.A >> 7;

    cpu->reg.A = (cpu->reg.A >> 1) | (old_c_flag << 7);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (cpu->reg.A >> 7) == 1;
}

void ROR()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = (v >> 1) | (cpu->reg.status_flag.C << 7);

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = (v >> 7);
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = (r >> 7) == 1;
}

void RTI()
{
    // TODO: Verify
    ++cpu->reg.SP;
    cpu->reg.P = read8(cpu->reg.SP + 0x100);

    cpu->reg.SP += 2;
    cpu->reg.PC = read16(cpu->reg.SP + 0x100);
    tick(3);
}

void RTS()
{
    /// TODO: Conflicting info on this op.
    /// some docs say that you increase the PC, others say you decrease it.
    /// increasing it seems to produce the correct output.
    cpu->reg.SP += 2;
    cpu->reg.PC = read16(cpu->reg.SP + 0x100) + 1;
    tick(4);
}

void STA()
{
    write8(cpu->oprand, cpu->reg.A);
}

void STX()
{
    write8(cpu->oprand, cpu->reg.X);
}

void STY()
{
    write8(cpu->oprand, cpu->reg.Y);
}


int execute(void)
{
    // 143 instructions.

    static int count = 0;
    //printf("%X  %X")
    cpu->opcode = read8(cpu->reg.PC++);
    printf("opcode: 0x%X PC: 0x%X SP: 0x%X count: %d\n", cpu->opcode, cpu->reg.PC, cpu->reg.SP, count);
    count++;

    switch (cpu->opcode)
    {
        case 0x00:
            NIP();
            break;
        case 0x01:
            address_load(AddressingType_IndirectZeroPageX);
            ORA();
            break;
        case 0x05:
            address_load(AddressingType_ZeroPage);
            ORA();
            break;
        case 0x06:
            address_load(AddressingType_ZeroPage);
            ASL();
            break;
        case 0x08:
            PHP();
            break;
        case 0x09:
            address_load(AddressingType_Immediate);
            ORA();
            break;
        case 0x0A:
            ASL_A();
            break;
        case 0x0D:
            address_load(AddressingType_Absolute);
            ORA();
            break;
        case 0x0E:
            address_load(AddressingType_Absolute);
            ASL();
            break;
        case 0x10:
            address_load(AddressingType_Relative);
            BPL();
            break;
        case 0x11:
            address_load(AddressingType_IndirectZeroPageY);
            ORA();
            break;
        case 0x15:
            address_load(AddressingType_ZeroPageX);
            ORA();
            break;
        case 0x16:
            address_load(AddressingType_ZeroPageX);
            ASL();
            break;
        case 0x18:
            CLC();
            break;
        case 0x19:
            address_load(AddressingType_AbsoluteY);
            ORA();
            break;
        case 0x1D:
            address_load(AddressingType_AbsoluteX);
            ORA();
            break;
        case 0x1E:
            NIP();
            break;
        case 0x20:
            address_load(AddressingType_Absolute);
            JSR();
            break;
        case 0x21:
            address_load(AddressingType_IndirectZeroPageX);
            AND();
            break;
        case 0x24:
            address_load(AddressingType_ZeroPage);
            BIT();
            break;
        case 0x25:
            address_load(AddressingType_ZeroPage);
            AND();
            break;
        case 0x26:
            address_load(AddressingType_ZeroPage);
            ROL();
            break;
        case 0x28:
            PLP();
            break;
        case 0x29:
            address_load(AddressingType_Immediate);
            AND();
            break;
        case 0x2A:
            ROL_A();
            break;
        case 0x2C:
            address_load(AddressingType_Absolute);
            BIT();
            break;
        case 0x2D:
            address_load(AddressingType_Absolute);
            AND();
            break;
        case 0x2E:
            address_load(AddressingType_Absolute);
            ROL();
            break;
        case 0x30:
            address_load(AddressingType_Relative);
            BMI();
            break;
        case 0x31:
            address_load(AddressingType_IndirectZeroPageY);
            AND();
            break;
        case 0x35:
            address_load(AddressingType_ZeroPageX);
            AND();
            break;
        case 0x36:
            address_load(AddressingType_ZeroPageX);
            ROL();
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
            address_load(AddressingType_AbsoluteX);
            ROL();
            break;
        case 0x40:
            RTI();
            break;
        case 0x41:
            address_load(AddressingType_IndirectZeroPageX);
            EOR();
            break;
        case 0x45:
            address_load(AddressingType_ZeroPage);
            EOR();
            break;
        case 0x46:
            address_load(AddressingType_ZeroPage);
            LSR();
            break;
        case 0x48:
            PHA();
            break;
        case 0x49:
            address_load(AddressingType_Immediate);
            EOR();
            break;
        case 0x4A:
            LSR();
            break;
        case 0x4C:
            address_load(AddressingType_Absolute);
            JMP();
            break;
        case 0x4D:
            address_load(AddressingType_Absolute);
            EOR();
            break;
        case 0x4E:
            address_load(AddressingType_Absolute);
            LSR();
            break;
        case 0x50:
            address_load(AddressingType_Relative);
            BVC();
            break;
        case 0x51:
            address_load(AddressingType_IndirectZeroPageY);
            EOR();
            break;
        case 0x55:
            address_load(AddressingType_ZeroPageX);
            EOR();
            break;
        case 0x56:
            address_load(AddressingType_ZeroPageX);
            LSR();
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
            address_load(AddressingType_AbsoluteX);
            LSR();
            break;
        case 0x60:
            RTS();
            break;
        case 0x61:
            address_load(AddressingType_IndirectZeroPageX);
            ADC();
            break;
        case 0x65:
            address_load(AddressingType_ZeroPage);
            ADC();
            break;
        case 0x66:
            address_load(AddressingType_ZeroPage);
            ROR();
            break;
        case 0x68:
            PLA();
            break;
        case 0x69:
            address_load(AddressingType_Immediate);
            ADC();
            break;
        case 0x6A:
            ROR_A();
            break;
        case 0x6C:
            NIP();
            break;
        case 0x6D:
            address_load(AddressingType_Absolute);
            ADC();
            break;
        case 0x6E:
            address_load(AddressingType_Absolute);
            ROR();
            break;
        case 0x70:
            address_load(AddressingType_Relative);
            BVS();
            break;
        case 0x71:
            address_load(AddressingType_IndirectZeroPageY);
            ADC();
            break;
        case 0x75:
            address_load(AddressingType_ZeroPageX);
            ADC();
            break;
        case 0x76:
            address_load(AddressingType_ZeroPageX);
            ROR();
            break;
        case 0x78:
            SEI();
            break;
        case 0x79:
            address_load(AddressingType_AbsoluteX);
            ADC();
            break;
        case 0x7D:
            address_load(AddressingType_AbsoluteX);
            ADC();
            break;
        case 0x7E:
            address_load(AddressingType_AbsoluteX);
            ROR();
            break;
        case 0x81:
            address_load(AddressingType_IndirectZeroPageX);
            STA();
            break;
        case 0x84:
            address_load(AddressingType_ZeroPage);
            STY();
            break;
        case 0x85:
            address_load(AddressingType_ZeroPage);
            STA();
            break;
        case 0x86:
            address_load(AddressingType_ZeroPage);
            STX();
            break;
        case 0x88:
            DEY();
            break;
        case 0x8A:
            TXA();
            break;
        case 0x8C:
            address_load(AddressingType_Absolute);
            STY();
            break;
        case 0x8D:
            address_load(AddressingType_Absolute);
            STA();
            break;
        case 0x8E:
            address_load(AddressingType_Absolute);
            STX();
            break;
        case 0x90:
            address_load(AddressingType_Relative);
            BCC();
            break;
        case 0x91:
            address_load(AddressingType_IndirectZeroPageY);
            STA();
            break;
        case 0x94:
            address_load(AddressingType_ZeroPageX);
            STY();
            break;
        case 0x95:
            address_load(AddressingType_ZeroPageX);
            STA();
            break;
        case 0x96:
            address_load(AddressingType_ZeroPageY);
            STX();
            break;
        case 0x98:
            TYA();
            break;
        case 0x99:
            address_load(AddressingType_AbsoluteY);
            STA();
            break;
        case 0x9A:
            TXS();
            break;
        case 0x9D:
            address_load(AddressingType_AbsoluteX);
            STA();
            break;
        case 0xA0:
            address_load(AddressingType_Immediate);
            LDY();
            break;
        case 0xA1:
            address_load(AddressingType_IndirectZeroPageX);
            LDA();
            break;
        case 0xA2:
            address_load(AddressingType_Immediate);
            LDX();
            break;
        case 0xA4:
            address_load(AddressingType_ZeroPage);
            LDY();
            break;
        case 0xA5:
            address_load(AddressingType_ZeroPage);
            LDA();
            break;
        case 0xA6:
            address_load(AddressingType_ZeroPage);
            LDX();
            break;
        case 0xA8:
            TAY();
            break;
        case 0xA9:
            address_load(AddressingType_Immediate);
            LDA();
            break;
        case 0xAA:
            TAX();
            break;
        case 0xAC:
            address_load(AddressingType_Absolute);
            LDY();
            break;
        case 0xAD:
            address_load(AddressingType_Absolute);
            LDA();
            break;
        case 0xAE:
            address_load(AddressingType_Absolute);
            LDX();
            break;
        case 0xB0:
            address_load(AddressingType_Relative);
            BCS();
            break;
        case 0xB1:
            address_load(AddressingType_IndirectZeroPageY);
            LDA();
            break;
        case 0xB4:
            address_load(AddressingType_ZeroPageX);
            LDY();
            break;
        case 0xB5:
            address_load(AddressingType_ZeroPageX);
            LDA();
            break;
        case 0xB6:
            NIP();
            break;
        case 0xB8:
            CLV();
            break;
        case 0xB9:
            address_load(AddressingType_AbsoluteY);
            LDA();
            break;
        case 0xBA:
            TSX();
            break;
        case 0xBC:
            address_load(AddressingType_AbsoluteX);
            LDY();
            break;
        case 0xBD:
            address_load(AddressingType_AbsoluteX);
            LDA();
            break;
        case 0xBE:
            address_load(AddressingType_AbsoluteY);
            LDX();
            break;
        case 0xC0:
            address_load(AddressingType_Immediate);
            CPY();
            break;
        case 0xC1:
            address_load(AddressingType_IndirectZeroPageX);
            CMP();
            break;
        case 0xC4:
            address_load(AddressingType_ZeroPage);
            CPY();
            break;
        case 0xC5:
            address_load(AddressingType_ZeroPage);
            CMP();
            break;
        case 0xC6:
            address_load(AddressingType_ZeroPage);
            DEC();
            break;
        case 0xC8:
            INY();
            break;
        case 0xC9:
            address_load(AddressingType_Immediate);
            CMP();
            break;
        case 0xCA:
            DEX();
            break;
        case 0xCC:
            address_load(AddressingType_Absolute);
            CPY();
            break;
        case 0xCD:
            address_load(AddressingType_Absolute);
            CMP();
            break;
        case 0xCE:
            address_load(AddressingType_Absolute);
            DEC();
            break;
        case 0xD0:
            address_load(AddressingType_Relative);
            BNE();
            break;
        case 0xD1:
            address_load(AddressingType_IndirectZeroPageY);
            CMP();
            break;
        case 0xD4:
            NIP();
            break;
        case 0xD5:
            address_load(AddressingType_ZeroPageX);
            CMP();
            break;
        case 0xD6:
            address_load(AddressingType_ZeroPageX);
            DEC();
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
            address_load(AddressingType_Immediate);
            CPX();
            break;
        case 0xE1:
            address_load(AddressingType_IndirectZeroPageX);
            SBC();
            break;
        case 0xE4:
            address_load(AddressingType_ZeroPage);
            CPX();
            break;
        case 0xE5:
            address_load(AddressingType_ZeroPage);
            SBC();
            break;
        case 0xE6:
            address_load(AddressingType_ZeroPage);
            INC();
            break;
        case 0xE8:
            INX();
            break;
        case 0xE9:
            address_load(AddressingType_Immediate);
            SBC();
            break;
        case 0xEA:
            NOP();
            break;
        case 0xEC:
            address_load(AddressingType_Absolute);
            CPX();
            break;
        case 0xED:
            address_load(AddressingType_Absolute);
            SBC();
            break;
        case 0xEE:
            address_load(AddressingType_Absolute);
            INC();
            break;
        case 0xF0:
            address_load(AddressingType_Relative);
            BEQ();
            break;
        case 0xF1:
            address_load(AddressingType_IndirectZeroPageY);
            SBC();
            break;
        case 0xF5:
            address_load(AddressingType_ZeroPageX);
            SBC();
            break;
        case 0xF6:
            address_load(AddressingType_ZeroPageX);
            INC();
            break;
        case 0xF8:
            SED();
            break;
        case 0xF9:
            address_load(AddressingType_AbsoluteY);
            SBC();
            break;
        case 0xFD:
            address_load(AddressingType_AbsoluteX);
            SBC();
            break;
        case 0xFE:
            NIP();
            break;
        default:
            NIP();
    }

    return 0;
}



/*
*   DEBUG
*/

void cpu_debug_step()
{
    execute();
}

cpu_t *cpu_debug_get()
{
    return cpu;
}