#ifndef MYLINUX_ARCH_H
#define MYLINUX_ARCH_H

#include <stdint.h>
#include <stddef.h>
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYLINUX_ARCH_NAME          "arm64"
#define MYLINUX_ARCH_BITS          64
#define MYLINUX_ARCH_LITTLE_ENDIAN 1

#define MYLINUX_ARCH_PAGE_SIZE     4096U
#define MYLINUX_ARCH_CACHE_LINE    64U

typedef uint64_t mylinux_register_t;
typedef uint64_t mylinux_address_t;
typedef uint64_t mylinux_physical_address_t;
typedef uint64_t mylinux_virtual_address_t;

typedef struct {
    mylinux_register_t x[31];
    mylinux_address_t stack_pointer;
    mylinux_address_t program_counter;
    mylinux_register_t processor_state;
} mylinux_arm64_register_context_t;

typedef struct {
    mylinux_address_t start;
    mylinux_address_t end;
} mylinux_address_range_t;

typedef enum {
    MYLINUX_ARCH_STATE_UNKNOWN = 0,
    MYLINUX_ARCH_STATE_USER,
    MYLINUX_ARCH_STATE_KERNEL,
    MYLINUX_ARCH_STATE_HALTED
} mylinux_arch_state_t;

static inline int
mylinux_arch_range_is_valid(const mylinux_address_range_t *range)
{
    if (range == NULL) {
        return 0;
    }

    if (range->end < range->start) {
        return 0;
    }

    return 1;
}

static inline uint64_t
mylinux_arch_range_size(const mylinux_address_range_t *range)
{
    if (!mylinux_arch_range_is_valid(range)) {
        return 0;
    }

    return range->end - range->start;
}

static inline int
mylinux_arch_address_is_aligned(
    mylinux_address_t address,
    size_t alignment)
{
    if (alignment == 0) {
        return 0;
    }

    return (address % alignment) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_ARCH_H */
