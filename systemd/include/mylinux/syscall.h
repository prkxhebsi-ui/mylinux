#ifndef MYLINUX_SYSCALL_H
#define MYLINUX_SYSCALL_H

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "mylinux_arch.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef long mylinux_syscall_result_t;
typedef long mylinux_syscall_number_t;

#define MYLINUX_SYSCALL_INVALID (-1L)

/* Generic syscall wrapper declarations. */
mylinux_syscall_result_t
mylinux_syscall0(mylinux_syscall_number_t number);

mylinux_syscall_result_t
mylinux_syscall1(
    mylinux_syscall_number_t number,
    uintptr_t arg1);

mylinux_syscall_result_t
mylinux_syscall2(
    mylinux_syscall_number_t number,
    uintptr_t arg1,
    uintptr_t arg2);

mylinux_syscall_result_t
mylinux_syscall3(
    mylinux_syscall_number_t number,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3);

/* Validate a syscall number before dispatching it. */
static inline int
mylinux_syscall_number_valid(mylinux_syscall_number_t number)
{
    if (number < 0) {
        return 0;
    } else {
        return 1;
    }
}

/* Clear a syscall argument buffer. */
static inline void
mylinux_syscall_clear_buffer(void *buffer, size_t size)
{
    if (buffer != NULL) {
        memset(buffer, 0, size);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_SYSCALL_H */
