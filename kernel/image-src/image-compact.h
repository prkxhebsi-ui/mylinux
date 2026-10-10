#ifndef MYLINUX_IMAGE_COMPAT_H
#define MYLINUX_IMAGE_COMPAT_H

#include <stdint.h>
#include "arm64_image.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYLINUX_COMPAT_OK                 0
#define MYLINUX_COMPAT_NULL_HEADER       (-1)
#define MYLINUX_COMPAT_BAD_MAGIC         (-2)
#define MYLINUX_COMPAT_BAD_SIZE          (-3)
#define MYLINUX_COMPAT_UNSUPPORTED_FLAGS (-4)

typedef struct {
    uint64_t minimum_image_size;
    uint64_t maximum_image_size;
    uint64_t supported_flags_mask;
} mylinux_image_compat_policy_t;

int mylinux_image_check_compatibility(
    const mylinux_arm64_image_header_t *header,
    const mylinux_image_compat_policy_t *policy);

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_IMAGE_COMPAT_H */
