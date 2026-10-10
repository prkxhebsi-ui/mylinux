#ifndef MYLINUX_ENDIAN_H
#define MYLINUX_ENDIAN_H

#include <stdint.h>

/* Byte-order constants for MyLinux source code. */

#define MYLINUX_ENDIAN_LITTLE 1234
#define MYLINUX_ENDIAN_BIG 4321

#if defined(BYTE_ORDER) && 
(BYTE_ORDER == ORDER_LITTLE_ENDIAN)
#define MYLINUX_BUILD_ENDIAN MYLINUX_ENDIAN_LITTLE
#elif defined(BYTE_ORDER) && 
(BYTE_ORDER == ORDER_BIG_ENDIAN)
#define MYLINUX_BUILD_ENDIAN MYLINUX_ENDIAN_BIG
#else
#define MYLINUX_BUILD_ENDIAN 0
#endif

static inline uint16_t mylinux_bswap16(uint16_t value)
{
return (uint16_t)((value << 8) | (value >> 8));
}

static inline uint32_t mylinux_bswap32(uint32_t value)
{
return ((value & UINT32_C(0x000000FF)) << 24) |
((value & UINT32_C(0x0000FF00)) << 8) |
((value & UINT32_C(0x00FF0000)) >> 8) |
((value & UINT32_C(0xFF000000)) >> 24);
}

static inline uint64_t mylinux_bswap64(uint64_t value)
{
return ((uint64_t)mylinux_bswap32((uint32_t)value) << 32) |
mylinux_bswap32((uint32_t)(value >> 32));
}

#endif /* MYLINUX_ENDIAN_H */

