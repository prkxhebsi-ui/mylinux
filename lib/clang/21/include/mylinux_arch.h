#ifndef MYLINUX_ARCH_H #define MYLINUX_ARCH_H

#include <stdint.h>

/* MyLinux architecture identification.

These constants describe targets; they do not detect hardware at runtime. */ 

#define MYLINUX_ARCH_UNKNOWN 0 #define MYLINUX_ARCH_ARM 1 #define MYLINUX_ARCH_AARCH64 2 #define MYLINUX_ARCH_X86 3 #define MYLINUX_ARCH_X86_64 4 #define MYLINUX_ARCH_RISCV32 5 #define MYLINUX_ARCH_RISCV64 6

#if defined(aarch64) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_AARCH64 #elif defined(arm) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_ARM #elif defined(x86_64) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_X86_64 #elif defined(i386) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_X86 #elif defined(__riscv) && (__riscv_xlen == 64) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_RISCV64 #elif defined(__riscv) && (__riscv_xlen == 32) #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_RISCV32 #else #define MYLINUX_BUILD_ARCH MYLINUX_ARCH_UNKNOWN #endif

typedef struct { uint32_t architecture; uint32_t pointer_bits; } mylinux_arch_info_t;

#if defined(SIZEOF_POINTER) #define MYLINUX_POINTER_BITS (SIZEOF_POINTER * 8) #else #define MYLINUX_POINTER_BITS 0 #endif

#endif /* MYLINUX_ARCH_H */

