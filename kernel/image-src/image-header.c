/*
 * MyLinux ARM64 Image Header Validator
 * Target: Samsung Galaxy Tab A 8.0 (SM-T295)
 *
 * Freestanding-friendly validation helpers.
 * This does not boot or load a kernel Image.
 */

#include "arm64_image.h"

int mylinux_image_header_validate(
    const mylinux_arm64_image_header_t *header)
{
    if (header == 0) {
        return -1;
    }

    if (header->magic != MYLINUX_ARM64_IMAGE_MAGIC) {
        return -2;
    }

    if (header->res2 != 0 ||
        header->res3 != 0 ||
        header->res4 != 0) {
        return -3;
    }

    if (header->res5 != 0) {
        return -4;
    }

    return 0;
}

int mylinux_image_header_get_size(
    const mylinux_arm64_image_header_t *header,
    uint64_t *image_size)
{
    if (header == 0 || image_size == 0) {
        return -1;
    }

    if (mylinux_image_header_validate(header) != 0) {
        return -2;
    }

    *image_size = header->image_size;
    return 0;
}

int mylinux_image_header_get_text_offset(
    const mylinux_arm64_image_header_t *header,
    uint64_t *text_offset)
{
    if (header == 0 || text_offset == 0) {
        return -1;
    }

    if (mylinux_image_header_validate(header) != 0) {
        return -2;
    }

    *text_offset = header->text_offset;
    return 0;
}

int mylinux_image_header_get_flags(
    const mylinux_arm64_image_header_t *header,
    uint64_t *flags)
{
    if (header == 0 || flags == 0) {
        return -1;
    }

    if (mylinux_image_header_validate(header) != 0) {
        return -2;
    }

    *flags = header->flags;
    return 0;
}
