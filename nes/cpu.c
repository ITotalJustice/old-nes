#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cpu.h"
#include "mmu.h"

/// TODO: Move these into a util.h
#define BIT0 (1)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)

/// For masking
#define MBIT0(x) ((x & BIT0))
#define MBIT1(x) ((x & BIT1))
#define MBIT2(x) ((x & BIT2))
#define MBIT3(x) ((x & BIT3))
#define MBIT4(x) ((x & BIT4))
#define MBIT5(x) ((x & BIT5))
#define MBIT6(x) ((x & BIT6))
#define MBIT7(x) ((x & BIT7))

/// Left-shift bit.
#define LBIT0(x) (x)
#define LBIT1(x) ((x << 1))
#define LBIT2(x) ((x << 2))
#define LBIT3(x) ((x << 3))
#define LBIT4(x) ((x << 4))
#define LBIT5(x) ((x << 5))
#define LBIT6(x) ((x << 6))
#define LBIT7(x) ((x << 7))

/// Right-shift bit.
#define RBIT0(x) (x)
#define RBIT1(x) ((x >> 1))
#define RBIT2(x) ((x >> 2))
#define RBIT3(x) ((x >> 3))
#define RBIT4(x) ((x >> 4))
#define RBIT5(x) ((x >> 5))
#define RBIT6(x) ((x >> 6))
#define RBIT7(x) ((x >> 7))

cpu_t *cpu = NULL;

static inline void tick(uint8_t c)
{
    assert(c > 0);
    cpu->cycle += c;
}

static inline uint8_t read8(uint16_t addr)
{
    tick(1);
    return mmu_read8(addr);
}

static inline uint16_t read16(uint16_t addr)
{
    tick(2);
    return mmu_read16(addr);
}

static inline void write8(uint16_t addr, uint8_t v)
{
    mmu_write8(addr, v);
    tick(1);
}

static inline void write16(uint16_t addr, uint16_t v)
{
    mmu_write16(addr, v);
    tick(2);
}

static inline void page_cross(uint16_t a, uint16_t b)
{
    if ((a & 0x0F00) != (b & 0x0F00)) tick(1);
}

/// https://youtu.be/fWqBmmPQP40?t=556
/// http://archive.6502.org/books/mcs6500_family_hardware_manual.pdf
static inline void addressing(AddrType type)
{
    switch (type)
    {
        /// The oprand is either reg A or not needed.
        case AddrType_Acc:
            tick(1);
            break;
        case AddrType_Imp:
            break;

        case AddrType_Rel:
            cpu->oprand = cpu->reg.PC++;
            //tick(1);
            break;
        case AddrType_Imm:
            cpu->oprand = cpu->reg.PC++;
            //tick(1);
            break;
        
        case AddrType_Abs:
            cpu->oprand = read16(cpu->reg.PC);
            cpu->reg.PC += 2;
            break;
        
        /// Extra clock cycle if page crossed.
        case AddrType_AbsX:
            page_cross(cpu->reg.PC, cpu->reg.PC + cpu->reg.X);
            cpu->oprand = read16(cpu->reg.PC) + cpu->reg.X;
            cpu->reg.PC += 2;
            break;
        case AddrType_AbsY:
            page_cross(cpu->reg.PC, cpu->reg.PC + cpu->reg.Y);
            cpu->oprand = read16(cpu->reg.PC) + cpu->reg.Y;
            cpu->reg.PC += 2;
            break;

        /// Zero page can only pull values between 0x00 - 0xFF.
        /// if the value is 0xFF >, then it wraps around.
        /// 2 + 0xFF = 0x01.
        /// The easy way to do this is to just cast the return value as a uint8_t.
        /// this will handle everything automatically.
        /// another way is to do % 256.
        /// that's probably(?) slower than a cast, and less obvious to me.
        /// especially because syntax highlighting makes it clear of a cast.
        /// I also imagine that *all* cpu's treat uint8 as a byte.
        case AddrType_ZP:
            cpu->oprand = read8(cpu->reg.PC++);
            break;
        case AddrType_ZPX:
            cpu->oprand = (uint8_t)(read8(cpu->reg.PC++) + cpu->reg.X);
            break;
        case AddrType_ZPY:
            cpu->oprand = (uint8_t)(read8(cpu->reg.PC++) + cpu->reg.Y);
            break;

        case AddrType_Ind:
            cpu->oprand = read16(cpu->reg.PC);
            cpu->reg.PC += 2;
            /// Due to a bug in the 6502 (fixed in CMOS), if the jmp oprand is 0xxxFF,
            /// this will cause the MSB to be read from 0xx00.
            /// so we mask 0xFFxx and read from there...
            cpu->oprand = (cpu->oprand & 0xFF) == 0xFF ? (read8(cpu->oprand & 0xFF00) << 8) | read8(cpu->oprand) : read16(cpu->oprand);
            break;

        case AddrType_IndZPX:
            cpu->oprand = read8(cpu->reg.PC++);
            cpu->oprand = (read8((uint8_t)(cpu->oprand + cpu->reg.X + 1)) << 8) | read8((uint8_t)(cpu->oprand + cpu->reg.X));
            tick(2);
            break;

        // Different to indirectX in that it does the lookup first, then adds Y.
        case AddrType_IndZPY:
            cpu->oprand = read8(cpu->reg.PC++);
            cpu->oprand = ((read8((uint8_t)(cpu->oprand + 1)) << 8) | read8(cpu->oprand)) + cpu->reg.Y;
            tick(2);
            break;
        
        default:
            printf("INCORRECT ADDRESS TYPE: %u", type);
            assert(0);
    }
}

void cpu_reset()
{
    /// TODO: check what other valus need to be set upon reset.
    cpu->reg.status_flag.I = true;
    cpu->reg.status_flag.U = true;

    /// Set registers.
    cpu->reg.A = false;
    cpu->reg.X = false;
    cpu->reg.Y = false;
    cpu->reg.SP -= 3;
    cpu->reg.PC = mmu_read16(0xFFFC);
}

void cpu_power_up()
{
    /// Set status flags.
    cpu->reg.status_flag.I = true;
    cpu->reg.status_flag.D = false;
    cpu->reg.status_flag.U = true;

    /// Set registers.
    cpu->reg.A = false;
    cpu->reg.X = false;
    cpu->reg.Y = false;
    cpu->reg.SP = 0xFD;
    cpu->reg.PC = mmu_read16(0xFFFC);

    /// TODO: REMOVE THIS
    /// This is for nestest auto.
    cpu->cycle = 7;
    cpu->reg.PC = 0xC000;
}

int cpu_init()
{
    cpu = malloc(sizeof(cpu_t));
    assert(cpu);

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
static inline void __ADC_SBC(uint8_t v)
{
    uint8_t old_carry_flag = cpu->reg.status_flag.C;
    uint8_t old_a_value = cpu->reg.A;

    cpu->reg.status_flag.C = (cpu->reg.A + v + cpu->reg.status_flag.C) > 0xFF;
    cpu->reg.A += v + old_carry_flag;

    /// BUG:
    /// Not set if decimal mode is enabled.
    /// fixed in CMOS.
    /// http://nesdev.com/6502bugs.txt
    //if (cpu->reg.status_flag.D == 0)
    {
        cpu->reg.status_flag.V = ((old_a_value ^ cpu->reg.A) & (v ^ cpu->reg.A) & BIT7) > 0;
        cpu->reg.status_flag.Z = cpu->reg.A == 0;
        cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;
    }

    tick(1);
}

static inline void ADC()
{
    __ADC_SBC(read8(cpu->oprand));
}

static inline void SBC()
{
    /// Comp the value for the reverse *big brain*
    __ADC_SBC(~read8(cpu->oprand));
}

static inline void AND()
{
    cpu->reg.A &= read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(1);
}

static inline void ASL_A()
{
    cpu->reg.status_flag.C = RBIT7(cpu->reg.A);

    cpu->reg.A <<= 1;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(2);
}

static inline void ASL()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v << 1;

    write8(cpu->oprand, r);

    /// http://www.obelisk.me.uk/6502/reference.html#ASL
    /// this incorrectly states that Z is set if A == 0.
    /// however it is set is the result == 0.
    cpu->reg.status_flag.C = RBIT7(v);
    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void BIT()
{
    uint8_t v = read8(cpu->oprand);

    cpu->reg.status_flag.Z = (cpu->reg.A & v) == 0;
    cpu->reg.status_flag.V = RBIT6(v) & 1;
    cpu->reg.status_flag.N = RBIT7(v);

    tick(1);
}

static inline void BRK()
{
    /// TODO: Finish this op.
    /// update tick count after finishing also.

    cpu->reg.status_flag.B = true;
    assert(0);

    tick(7);
}


/*
*   Branch Time!
*/
/// TODO: correct tick()
/// all branch are 2 cycles, + 1 if taken, + 1 if page cross.

static inline void __BRANCH(bool cond)
{
    int8_t v = read8(cpu->oprand);
    if (cond)
    {
        page_cross(cpu->reg.PC, cpu->reg.PC + v);
        cpu->reg.PC += v;
        tick(1);
    }
    tick(1);
}

static inline void BCC()
{
    __BRANCH(cpu->reg.status_flag.C == 0);
}

static inline void BCS()
{
    __BRANCH(cpu->reg.status_flag.C == 1);
}

static inline void BEQ()
{
    __BRANCH(cpu->reg.status_flag.Z == 1);
}

static inline void BNE()
{
    __BRANCH(cpu->reg.status_flag.Z == 0);
}

static inline void BMI()
{
    __BRANCH(cpu->reg.status_flag.N == 1);
}

static inline void BPL()
{
    __BRANCH(cpu->reg.status_flag.N == 0);
}

static inline void BVS()
{
    __BRANCH(cpu->reg.status_flag.V == 1);
}

static inline void BVC()
{
    __BRANCH(cpu->reg.status_flag.V == 0);
}


/*
*   Flag Instructions
*/
static inline void CLC()
{
    cpu->reg.status_flag.C = false;
    tick(2);
}

static inline void CLI()
{
    cpu->reg.status_flag.I = false;
    tick(2);
}

static inline void CLV()
{
    cpu->reg.status_flag.V = false;
    tick(2);
}

static inline void CLD()
{
    cpu->reg.status_flag.D = false;
    tick(2);
}

static inline void SEC()
{
    cpu->reg.status_flag.C = true;
    tick(2);
}

static inline void SEI()
{
    cpu->reg.status_flag.I = true;
    tick(2);
}

static inline void SED()
{
    cpu->reg.status_flag.D = true;
    tick(2);
}


static inline void CMP()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.A - v;

    cpu->reg.status_flag.C = (cpu->reg.A >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(1);
}

static inline void CPX()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.X - v;

    cpu->reg.status_flag.C = (cpu->reg.X >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(1);
}

static inline void CPY()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = cpu->reg.Y - v;

    cpu->reg.status_flag.C = (cpu->reg.Y >= v);
    cpu->reg.status_flag.Z = r == 0;
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(1);
}

static inline void DEC()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v - 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void EOR()
{
    cpu->reg.A ^= read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(1);
}

static inline void INC()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v + 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void LDA()
{
    cpu->reg.A = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(1);
}

static inline void LDX()
{
    cpu->reg.X = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.X) == 1;

    tick(1);
}

static inline void LDY()
{
    cpu->reg.Y = read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.Y) == 1;

    tick(1);
}

static inline void LSR_A()
{
    cpu->reg.status_flag.C = cpu->reg.A & 1;

    cpu->reg.A >>= 1;
    
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(2);
}

static inline void LSR()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = v >> 1;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = v & 1;
    cpu->reg.status_flag.Z = r == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void NOP()
{
    tick(2);
}

static inline void ORA()
{
    cpu->reg.A |= read8(cpu->oprand);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(1);
}


/*
*   Stack Stuff
*/
#define STACK_ADDR 0x100
static inline void push_stack8(uint8_t v)
{
    write8(cpu->reg.SP + STACK_ADDR, v);
    --cpu->reg.SP;
}

static inline void push_stack16(uint16_t v)
{
    push_stack8(v >> 8);
    push_stack8(v & 0xFF);
}

static inline uint8_t pull_stack8()
{
    cpu->reg.SP++;
    return read8(cpu->reg.SP + STACK_ADDR);
}

static inline uint16_t pull_stack16()
{
   return (pull_stack8()) | (pull_stack8() << 8);
}

static inline void JMP()
{
    cpu->reg.PC = cpu->oprand;
    tick(1);
}

static inline void PHA()
{
    push_stack8(cpu->reg.A);
    tick(2);
}

static inline void PHP()
{
    /// breakpoint bit is set.
    /// http://nesdev.com/6502bugs.txt
    push_stack8(cpu->reg.P | BIT4);
    tick(2);
}

static inline void PLA()
{
    cpu->reg.A = pull_stack8();

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(3);
}

static inline void PLP()
{
    cpu->reg.P = pull_stack8();

    cpu->reg.P &= ~BIT4;
    cpu->reg.P |= BIT5;

    tick(3);
}

/// JSR-RTS-RTI-BUG: http://nesdev.com/6502bugs.txt
static inline void JSR()
{
    /// JSR has a bug that sets the addr at -1.
    /// This is *fixed* in RTS, but not in RTI.
    push_stack16(cpu->reg.PC - 1);
    cpu->reg.PC = cpu->oprand;
    tick(2);
}

static inline void RTS()
{
    /// Due to JSR bug, the PC was set at PC -1.
    /// RTS increases the PC back.
    cpu->reg.PC = pull_stack16() + 1;
    tick(4);
}

static inline void RTI()
{
    cpu->reg.P = pull_stack8();

    cpu->reg.P &= ~BIT4;
    cpu->reg.P |= BIT5;

    /// Due to the JSR bug, the PC returned will be -1.
    /// Obviously we don't need t0 decriment it as JSR already did that...
    cpu->reg.PC = pull_stack16();
    tick(3);
}


/*
*   Register Instructions.
*/
static inline void TAX()
{
    cpu->reg.X = cpu->reg.A;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.X) == 1; 

    tick(2);
}

static inline void TXA()
{
    cpu->reg.A = cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1; 

    tick(2);
}

static inline void TSX()
{
    cpu->reg.X = cpu->reg.SP;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.X) == 1; 

    tick(2);
}

static inline void TXS()
{
    cpu->reg.SP = cpu->reg.X;

    tick(2);
}

static inline void DEX()
{
    --cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.X) == 1; 

    tick(2);
}

static inline void INX()
{
    ++cpu->reg.X;

    cpu->reg.status_flag.Z = cpu->reg.X == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.X) == 1; 

    tick(2);
}

static inline void TAY()
{
    cpu->reg.Y = cpu->reg.A;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.Y) == 1; 

    tick(2);
}

static inline void TYA()
{
    cpu->reg.A = cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1; 

    tick(2);
}

static inline void DEY()
{
    --cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.Y) == 1; 

    tick(2);
}

static inline void INY()
{
    ++cpu->reg.Y;

    cpu->reg.status_flag.Z = cpu->reg.Y == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.Y) == 1; 

    tick(2);
}

static inline void ROL_A()
{
    uint8_t old_c_flag = cpu->reg.status_flag.C;
    cpu->reg.status_flag.C = RBIT7(cpu->reg.A);

    cpu->reg.A = (cpu->reg.A << 1) | old_c_flag;

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(2);
}

static inline void ROL()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = (v << 1) | cpu->reg.status_flag.C;

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = RBIT7(v);
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void ROR_A()
{
    uint8_t old_c_flag = cpu->reg.status_flag.C;
    cpu->reg.status_flag.C = cpu->reg.A & 1;

    cpu->reg.A = (cpu->reg.A >> 1) | (old_c_flag << 7);

    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(cpu->reg.A) == 1;

    tick(2);
}

static inline void ROR()
{
    uint8_t v = read8(cpu->oprand);
    uint8_t r = (v >> 1) | (cpu->reg.status_flag.C << 7);

    write8(cpu->oprand, r);

    cpu->reg.status_flag.C = v & 1;
    cpu->reg.status_flag.Z = cpu->reg.A == 0; 
    cpu->reg.status_flag.N = RBIT7(r) == 1;

    tick(2);
}

static inline void STA()
{
    write8(cpu->oprand, cpu->reg.A);

    tick(1);
}

static inline void STX()
{
    write8(cpu->oprand, cpu->reg.X);

    tick(1);
}

static inline void STY()
{
    write8(cpu->oprand, cpu->reg.Y);

    tick(1);
}


int execute(void)
{
    /// 149 instructions so far...

    cpu->opcode = mmu_read8(cpu->reg.PC);

    static int count = 0;
    printf("%04X   %02X %04X \tA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%u count:%d\n",
    cpu->reg.PC, cpu->opcode, cpu->oprand, cpu->reg.A, cpu->reg.X, cpu->reg.Y, cpu->reg.P, cpu->reg.SP, cpu->cycle, count);

    count++;
    cpu->reg.PC++;

    switch (cpu->opcode)
    {
        case 0x00: NIP(); break;
        case 0x01: addressing(AddrType_IndZPX); ORA();      break;
        case 0x05: addressing(AddrType_ZP);     ORA();      break;
        case 0x06: addressing(AddrType_ZP);     ASL();      break;
        case 0x08: addressing(AddrType_Imp);    PHP();      break;
        case 0x09: addressing(AddrType_Imm);    ORA();      break;
        case 0x0A: addressing(AddrType_Acc);    ASL_A();    break;
        case 0x0D: addressing(AddrType_Abs);    ORA();      break;
        case 0x0E: addressing(AddrType_Abs);    ASL();      break;
        case 0x10: addressing(AddrType_Rel);    BPL();      break;
        case 0x11: addressing(AddrType_IndZPY); ORA();      break;
        case 0x15: addressing(AddrType_ZPX);    ORA();      break;
        case 0x16: addressing(AddrType_ZPX);    ASL();      break;
        case 0x18: addressing(AddrType_Imp);    CLC();      break;
        case 0x19: addressing(AddrType_AbsY);   ORA();      break;
        case 0x1D: addressing(AddrType_AbsX);   ORA();      break;
        case 0x1E: addressing(AddrType_AbsX);   ASL();      break;
        case 0x20: addressing(AddrType_Abs);    JSR();      break;
        case 0x21: addressing(AddrType_IndZPX); AND();      break;
        case 0x24: addressing(AddrType_ZP);     BIT();      break;
        case 0x25: addressing(AddrType_ZP);     AND();      break;
        case 0x26: addressing(AddrType_ZP);     ROL();      break;
        case 0x28: addressing(AddrType_Imp);    PLP();      break;
        case 0x29: addressing(AddrType_Imm);    AND();      break;
        case 0x2A: addressing(AddrType_Acc);    ROL_A();    break;
        case 0x2C: addressing(AddrType_Abs);    BIT();      break;
        case 0x2D: addressing(AddrType_Abs);    AND();      break;
        case 0x2E: addressing(AddrType_Abs);    ROL();      break;
        case 0x30: addressing(AddrType_Rel);    BMI();      break;
        case 0x31: addressing(AddrType_IndZPY); AND();      break;
        case 0x35: addressing(AddrType_ZPX);    AND();      break;
        case 0x36: addressing(AddrType_ZPX);    ROL();      break;
        case 0x38: addressing(AddrType_Imp);    SEC();      break;
        case 0x39: addressing(AddrType_AbsY);   AND();      break;
        case 0x3D: addressing(AddrType_AbsX);   AND();      break;
        case 0x3E: addressing(AddrType_AbsX);   ROL();      break;
        case 0x40: addressing(AddrType_Imp);    RTI();      break;
        case 0x41: addressing(AddrType_IndZPX); EOR();      break;
        case 0x45: addressing(AddrType_ZP);     EOR();      break;
        case 0x46: addressing(AddrType_ZP);     LSR();      break;
        case 0x48: addressing(AddrType_Imp);    PHA();      break;
        case 0x49: addressing(AddrType_Imm);    EOR();      break;
        case 0x4A: addressing(AddrType_Acc);    LSR_A();    break;
        case 0x4C: addressing(AddrType_Abs);    JMP();      break;
        case 0x4D: addressing(AddrType_Abs);    EOR();      break;
        case 0x4E: addressing(AddrType_Abs);    LSR();      break;
        case 0x50: addressing(AddrType_Rel);    BVC();      break;
        case 0x51: addressing(AddrType_IndZPY); EOR();      break;
        case 0x55: addressing(AddrType_ZPX);    EOR();      break;
        case 0x56: addressing(AddrType_ZPX);    LSR();      break;
        case 0x58: addressing(AddrType_Imp);    CLI();      break;
        case 0x59: addressing(AddrType_AbsY);   EOR();      break;
        case 0x5D: addressing(AddrType_AbsX);   EOR();      break;
        case 0x5E: addressing(AddrType_AbsX);   LSR();      break;
        case 0x60: addressing(AddrType_Imp);    RTS();      break;
        case 0x61: addressing(AddrType_IndZPX); ADC();      break;
        case 0x65: addressing(AddrType_ZP);     ADC();      break;
        case 0x66: addressing(AddrType_ZP);     ROR();      break;
        case 0x68: addressing(AddrType_Imp);    PLA();      break;
        case 0x69: addressing(AddrType_Imm);    ADC();      break;
        case 0x6A: addressing(AddrType_Acc);    ROR_A();    break;
        case 0x6C: addressing(AddrType_Ind);    JMP();      break;
        case 0x6D: addressing(AddrType_Abs);    ADC();      break;
        case 0x6E: addressing(AddrType_Abs);    ROR();      break;
        case 0x70: addressing(AddrType_Rel);    BVS();      break;
        case 0x71: addressing(AddrType_IndZPY); ADC();      break;
        case 0x75: addressing(AddrType_ZPX);    ADC();      break;
        case 0x76: addressing(AddrType_ZPX);    ROR();      break;
        case 0x78: addressing(AddrType_Imp);    SEI();      break;
        case 0x79: addressing(AddrType_AbsY);   ADC();      break;
        case 0x7D: addressing(AddrType_AbsX);   ADC();      break;
        case 0x7E: addressing(AddrType_AbsX);   ROR();      break;
        case 0x81: addressing(AddrType_IndZPX); STA();      break;
        case 0x84: addressing(AddrType_ZP);     STY();      break;
        case 0x85: addressing(AddrType_ZP);     STA();      break;
        case 0x86: addressing(AddrType_ZP);     STX();      break;
        case 0x88: addressing(AddrType_Imp);    DEY();      break;
        case 0x8A: addressing(AddrType_Imp);    TXA();      break;
        case 0x8C: addressing(AddrType_Abs);    STY();      break;
        case 0x8D: addressing(AddrType_Abs);    STA();      break;
        case 0x8E: addressing(AddrType_Abs);    STX();      break;
        case 0x90: addressing(AddrType_Rel);    BCC();      break;
        case 0x91: addressing(AddrType_IndZPY); STA();      break;
        case 0x94: addressing(AddrType_ZPX);    STY();      break;
        case 0x95: addressing(AddrType_ZPX);    STA();      break;
        case 0x96: addressing(AddrType_ZPY);    STX();      break;
        case 0x98: addressing(AddrType_Imp);    TYA();      break;
        case 0x99: addressing(AddrType_AbsY);   STA();      break;
        case 0x9A: addressing(AddrType_Imp);    TXS();      break;
        case 0x9D: addressing(AddrType_AbsX);   STA();      break;
        case 0xA0: addressing(AddrType_Imm);    LDY();      break;
        case 0xA1: addressing(AddrType_IndZPX); LDA();      break;
        case 0xA2: addressing(AddrType_Imm);    LDX();      break;
        case 0xA4: addressing(AddrType_ZP);     LDY();      break;
        case 0xA5: addressing(AddrType_ZP);     LDA();      break;
        case 0xA6: addressing(AddrType_ZP);     LDX();      break;
        case 0xA8: addressing(AddrType_Imp);    TAY();      break;
        case 0xA9: addressing(AddrType_Imm);    LDA();      break;
        case 0xAA: addressing(AddrType_Imp);    TAX();      break;
        case 0xAC: addressing(AddrType_Abs);    LDY();      break;
        case 0xAD: addressing(AddrType_Abs);    LDA();      break;
        case 0xAE: addressing(AddrType_Abs);    LDX();      break;
        case 0xB0: addressing(AddrType_Rel);    BCS();      break;
        case 0xB1: addressing(AddrType_IndZPY); LDA();      break;
        case 0xB4: addressing(AddrType_ZPX);    LDY();      break;
        case 0xB5: addressing(AddrType_ZPX);    LDA();      break;
        case 0xB6: addressing(AddrType_ZPY);    LDX();      break;
        case 0xB8: addressing(AddrType_Imp);    CLV();      break;
        case 0xB9: addressing(AddrType_AbsY);   LDA();      break;
        case 0xBA: addressing(AddrType_Imp);    TSX();      break;
        case 0xBC: addressing(AddrType_AbsX);   LDY();      break;
        case 0xBD: addressing(AddrType_AbsX);   LDA();      break;
        case 0xBE: addressing(AddrType_AbsY);   LDX();      break;
        case 0xC0: addressing(AddrType_Imm);    CPY();      break;
        case 0xC1: addressing(AddrType_IndZPX); CMP();      break;
        case 0xC4: addressing(AddrType_ZP);     CPY();      break;
        case 0xC5: addressing(AddrType_ZP);     CMP();      break;
        case 0xC6: addressing(AddrType_ZP);     DEC();      break;
        case 0xC8: addressing(AddrType_Imp);    INY();      break;
        case 0xC9: addressing(AddrType_Imm);    CMP();      break;
        case 0xCA: addressing(AddrType_Imp);    DEX();      break;
        case 0xCC: addressing(AddrType_Abs);    CPY();      break;
        case 0xCD: addressing(AddrType_Abs);    CMP();      break;
        case 0xCE: addressing(AddrType_Abs);    DEC();      break;
        case 0xD0: addressing(AddrType_Rel);    BNE();      break;
        case 0xD1: addressing(AddrType_IndZPY); CMP();      break;
        case 0xD5: addressing(AddrType_ZPX);    CMP();      break;
        case 0xD6: addressing(AddrType_ZPX);    DEC();      break;
        case 0xD8: addressing(AddrType_Imp);    CLD();      break;
        case 0xD9: addressing(AddrType_AbsY);   CMP();      break;
        case 0xDD: addressing(AddrType_AbsX);   CMP();      break;
        case 0xDE: addressing(AddrType_AbsX);   DEC();      break;
        case 0xE0: addressing(AddrType_Imm);    CPX();      break;
        case 0xE1: addressing(AddrType_IndZPX); SBC();      break;
        case 0xE4: addressing(AddrType_ZP);     CPX();      break;
        case 0xE5: addressing(AddrType_ZP);     SBC();      break;
        case 0xE6: addressing(AddrType_ZP);     INC();      break;
        case 0xE8: addressing(AddrType_Imp);    INX();      break;
        case 0xE9: addressing(AddrType_Imm);    SBC();      break;
        case 0xEA: addressing(AddrType_Imp);    NOP();      break;
        case 0xEC: addressing(AddrType_Abs);    CPX();      break;
        case 0xED: addressing(AddrType_Abs);    SBC();      break;
        case 0xEE: addressing(AddrType_Abs);    INC();      break;
        case 0xF0: addressing(AddrType_Rel);    BEQ();      break;
        case 0xF1: addressing(AddrType_IndZPY); SBC();      break;
        case 0xF5: addressing(AddrType_ZPX);    SBC();      break;
        case 0xF6: addressing(AddrType_ZPX);    INC();      break;
        case 0xF8: addressing(AddrType_Imp);    SED();      break;
        case 0xF9: addressing(AddrType_AbsY);   SBC();      break;
        case 0xFD: addressing(AddrType_AbsX);   SBC();      break;
        case 0xFE: addressing(AddrType_AbsX);   INC();      break;

        /// ERROR. Will assert(0).
        default: NIP();
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