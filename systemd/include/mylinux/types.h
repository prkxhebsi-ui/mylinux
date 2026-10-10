#ifndef MYLINUX_TYPES_H
#define MYLINUX_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int8_t   mylinux_i8;
typedef uint8_t  mylinux_u8;
typedef int16_t  mylinux_i16;
typedef uint16_t mylinux_u16;
typedef int32_t  mylinux_i32;
typedef uint32_t mylinux_u32;
typedef int64_t  mylinux_i64;
typedef uint64_t mylinux_u64;

typedef intptr_t  mylinux_intptr;
typedef uintptr_t mylinux_uintptr;
typedef size_t    mylinux_size;
typedef ssize_t   mylinux_ssize;
typedef time_t    mylinux_time;

typedef int mylinux_status_t;

#define MYLINUX_STATUS_OK          0
#define MYLINUX_STATUS_ERROR      -1
#define MYLINUX_STATUS_INVALID    -2
#define MYLINUX_STATUS_UNSUPPORTED -3
#define MYLINUX_STATUS_TIMEOUT    -4
#define MYLINUX_STATUS_NO_MEMORY  -5

#define MYLINUX_ARRAY_COUNT(array) \
    (sizeof(array) / sizeof((array)[0]))

#define MYLINUX_UNUSED(value) ((void)(value))

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_TYPES_H */
