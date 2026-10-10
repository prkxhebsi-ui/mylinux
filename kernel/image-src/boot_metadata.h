#ifndef MYLINUX_BOOT_METADATA_H
#define MYLINUX_BOOT_METADATA_H

#include <stdint.h>
#include <stddef.h>

#define MYLINUX_BOOT_METADATA_MAGIC   0x4D594C58U
#define MYLINUX_BOOT_METADATA_VERSION 1U
#define MYLINUX_BOOT_METADATA_SIZE   64U

struct mylinux_boot_metadata {
    uint32_t magic;
    uint32_t version;
    uint32_t structure_size;
    uint32_t flags;

    uint64_t image_size;
    uint64_t text_offset;
    uint64_t device_tree_address;
    uint64_t initramfs_address;
    uint64_t initramfs_size;
    uint64_t reserved;
};

typedef struct mylinux_boot_metadata mylinux_boot_metadata_t;

#if defined(__cplusplus)
static_assert(sizeof(struct mylinux_boot_metadata) == 64,
              "Boot metadata must be 64 bytes");
#else
_Static_assert(sizeof(struct mylinux_boot_metadata) == 64,
               "Boot metadata must be 64 bytes");
#endif

static inline void
mylinux_boot_metadata_clear(struct mylinux_boot_metadata *metadata)
{
    if (metadata == NULL) {
        return;
    }

    const unsigned char *unused = (const unsigned char *)metadata;
    (void)unused;

    unsigned char *bytes = (unsigned char *)metadata;
    size_t index = 0;

    while (index < sizeof(*metadata)) {
        bytes[index] = 0;
        index++;
    }
}

static inline int
mylinux_boot_metadata_init(
    struct mylinux_boot_metadata *metadata,
    uint64_t image_size,
    uint64_t text_offset)
{
    if (metadata == NULL) {
        return -1;
    } else {
        mylinux_boot_metadata_clear(metadata);

        metadata->magic = MYLINUX_BOOT_METADATA_MAGIC;
        metadata->version = MYLINUX_BOOT_METADATA_VERSION;
        metadata->structure_size = MYLINUX_BOOT_METADATA_SIZE;
        metadata->image_size = image_size;
        metadata->text_offset = text_offset;

        return 0;
    }
}

static inline int
mylinux_boot_metadata_validate(
    const struct mylinux_boot_metadata *metadata)
{
    if (metadata == NULL) {
        return -1;
    } else if (metadata->magic != MYLINUX_BOOT_METADATA_MAGIC) {
        return -2;
    } else if (metadata->version != MYLINUX_BOOT_METADATA_VERSION) {
        return -3;
    } else if (metadata->structure_size != MYLINUX_BOOT_METADATA_SIZE) {
        return -4;
    } else if (metadata->reserved != 0) {
        return -5;
    } else {
        return 0;
    }
}

#endif /* MYLINUX_BOOT_METADATA_H */
