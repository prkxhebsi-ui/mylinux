#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>

#include "../include/mylinux/syscall.h"
#include "../include/mylinux/mylinux_arch.h"
#include "../include/mylinux/types.h"

/*
 * MyLinux syscall wrapper implementation.
 *
 * These wrappers call Linux syscalls through libc's syscall()
 * function. They do not add new kernel syscalls.
 *
 * Build as a userspace component, not as kernel code.
 */

static mylinux_syscall_result_t
mylinux_syscall_dispatch(
    mylinux_syscall_number_t number,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3,
    uintptr_t arg4,
    uintptr_t arg5,
    uintptr_t arg6,
    unsigned int argument_count)
{
    long result;

    if (!mylinux_syscall_number_valid(number)) {
        errno = EINVAL;
        return -1;
    }

    switch (argument_count) {
        case 0:
            result = syscall(number);
            break;

        case 1:
            result = syscall(number, arg1);
            break;

        case 2:
            result = syscall(number, arg1, arg2);
            break;

        case 3:
            result = syscall(number, arg1, arg2, arg3);
            break;

        case 4:
            result = syscall(number, arg1, arg2, arg3, arg4);
            break;

        case 5:
            result = syscall(number, arg1, arg2, arg3, arg4, arg5);
            break;

        case 6:
            result = syscall(
                number, arg1, arg2, arg3, arg4, arg5, arg6);
            break;

        default:
            errno = EINVAL;
            return -1;
    }

    return (mylinux_syscall_result_t)result;
}

mylinux_syscall_result_t
mylinux_syscall0(mylinux_syscall_number_t number)
{
    return mylinux_syscall_dispatch(
        number, 0, 0, 0, 0, 0, 0, 0);
}

mylinux_syscall_result_t
mylinux_syscall1(
    mylinux_syscall_number_t number,
    uintptr_t arg1)
{
    return mylinux_syscall_dispatch(
        number, arg1, 0, 0, 0, 0, 0, 1);
}

mylinux_syscall_result_t
mylinux_syscall2(
    mylinux_syscall_number_t number,
    uintptr_t arg1,
    uintptr_t arg2)
{
    return mylinux_syscall_dispatch(
        number, arg1, arg2, 0, 0, 0, 0, 2);
}

mylinux_syscall_result_t
mylinux_syscall3(
    mylinux_syscall_number_t number,
    uintptr_t arg1,
    uintptr_t arg2,
    uintptr_t arg3)
{
    return mylinux_syscall_dispatch(
        number, arg1, arg2, arg3, 0, 0, 0, 3);
}
