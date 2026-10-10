#ifndef MYLINUX_BOOT_PARAMS_H
#define MYLINUX_BOOT_PARAMS_H

#include <stdint.h>

#define MYLINUX_BOOT_PARAMS_VERSION 1U

#define MYLINUX_BOOT_FLAG_DEVICE_TREE (1U << 0)
#define MYLINUX_BOOT_FLAG_INITRAMFS   (1U << 1)
#define MYLINUX_BOOT_FLAG_CMDLINE     (1U << 2)

typedef struct {
    uint32_t version;
    uint32_t structure_size;
    uint64_t flags;

    uint64_t device_tree_address;
    uint64_t initramfs_address;
    uint64_t initramfs_size;
    uint64_t command_line_address;
} mylinux_boot_params_t;

#endif /* MYLINUX_BOOT_PARAMS_H */
