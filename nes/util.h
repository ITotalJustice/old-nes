#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif