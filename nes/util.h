/*
*   TotalJustice
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/// For masking
#define BIT0 (1)
#define BIT1 (1 << 1)
#define BIT2 (1 << 2)
#define BIT3 (1 << 3)
#define BIT4 (1 << 4)
#define BIT5 (1 << 5)
#define BIT6 (1 << 6)
#define BIT7 (1 << 7)

/// Masking
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


#define _1KB 1000
#define _2KB _1KB << 1
#define _4KB _2KB << 1
#define _8KB _4KB << 1
#define _16KB _8KB << 1
#define _32KB _16KB << 1
#define _64KB _32KB << 1
#define _128KB _64KB << 1
#define _256KB _128KB << 1
#define _512KB _256KB << 1

#define _1MB _512KB << 1
#define _2MB _1MB << 1
#define _4MB _2MB << 1
#define _8MB _4MB << 1
#define _16MB _8MB << 1
#define _32MB _16MB << 1
#define _64MB _32MB << 1
#define _128MB _64MB << 1
#define _256MB _128MB << 1
#define _512MB _256MB << 1


#define _1KiB 1024
#define _2KiB _1KiB << 1
#define _4KiB _2KiB << 1
#define _8KiB _4KiB << 1
#define _16KiB _8KiB << 1
#define _32KiB _16KiB << 1
#define _64KiB _32KiB << 1
#define _128KiB _64KiB << 1
#define _256KiB _128KiB << 1
#define _512KiB _256KiB << 1

#define _1MiB _512KiB << 1
#define _2MiB _1MiB << 1
#define _4MiB _2MiB << 1
#define _8MiB _4MiB << 1
#define _16MiB _8MiB << 1
#define _32MiB _16MiB << 1
#define _64MiB _32MiB << 1
#define _128MiB _64MiB << 1
#define _256MiB _128MiB << 1
#define _512MiB _256MiB << 1


inline const char *bool_str(bool cond) { return cond ? "true" : "false"; }

#ifdef __cplusplus
}
#endif