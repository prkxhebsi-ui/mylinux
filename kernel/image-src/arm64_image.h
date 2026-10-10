#ifndef MYLINUX_ARM64_IMAGE_H
#define MYLINUX_ARM64_IMAGE_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*
 * MyLinux ARM64 Image header definitions.
 *
 * Based on the Linux ARM64 Image header layout.
 * The actual kernel Image begins with this header.
 *
 * This header describes the image; it does not implement
 * the complete Linux boot protocol.
 */

#if !defined(__BYTE_ORDER__) || \
    !defined(__ORDER_LITTLE_ENDIAN__)
#error "Compiler must provide byte-order definitions"
#endif

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "Build this ARM64 Image header code for little-endian ARM64"
#endif

#define MYLINUX_ARM64_IMAGE_MAGIC  UINT32_C(0x644d5241)
#define MYLINUX_ARM64_IMAGE_MAGIC_TEXT "ARM\x64"

#define MYLINUX_ARM64_HEADER_SIZE 64U

typedef uint32_t mylinux_u32;
typedef uint64_t mylinux_u64;

typedef struct {
    mylinux_u32 code0;       /* Executable code */
    mylinux_u32 code1;       /* Executable code */

    mylinux_u64 text_offset; /* Image load offset, little endian */
    mylinux_u64 image_size;  /* Effective Image size, little endian */
    mylinux_u64 flags;       /* Kernel flags, little endian */

    mylinux_u64 res2;        /* Reserved: must be zero */
    mylinux_u64 res3;        /* Reserved: must be zero */
    mylinux_u64 res4;        /* Reserved: must be zero */

    mylinux_u32 magic;       /* 0x644d5241: "ARM\x64" */
    mylinux_u32 res5;        /* Reserved / PE COFF offset */
} mylinux_arm64_image_header_t;

/* Validate the expected structure layout at compile time. */
#if defined(__cplusplus)
static_assert(sizeof(mylinux_arm64_image_header_t) == 56,
              "Unexpected ARM64 header structure size");
static_assert(offsetof(mylinux_arm64_image_header_t, text_offset) == 8,
              "Unexpected text_offset offset");
static_assert(offsetof(mylinux_arm64_image_header_t, magic) == 48,
              "Unexpected magic offset");
#else
_Static_assert(sizeof(mylinux_arm64_image_header_t) == 56,
               "Unexpected ARM64 header structure size");
_Static_assert(offsetof(mylinux_arm64_image_header_t, text_offset) == 8,
               "Unexpected text_offset offset");
_Static_assert(offsetof(mylinux_arm64_image_header_t, magic) == 48,
               "Unexpected magic offset");
#endif

static inline int
mylinux_arm64_image_has_magic(
    const mylinux_arm64_image_header_t *header)
{
    if (header == NULL) {
        return 0;
    }

    return header->magic == MYLINUX_ARM64_IMAGE_MAGIC;
}

static inline int
mylinux_arm64_image_reserved_fields_are_zero(
    const mylinux_arm64_image_header_t *header)
{
    if (header == NULL) {
        return 0;
    }

    return header->res2 == 0 &&
           header->res3 == 0 &&
           header->res4 == 0;
}

static inline int
mylinux_arm64_image_header_is_valid(
    const mylinux_arm64_image_header_t *header)
{
    if (header == NULL) {
        return 0;
    }

    if (!mylinux_arm64_image_has_magic(header)) {
        return 0;
    }

    if (!mylinux_arm64_image_reserved_fields_are_zero(header)) {
        return 0;
    }

    return 1;
}

#endif /* MYLINUX_ARM64_IMAGE_H */
