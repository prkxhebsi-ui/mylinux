// SPDX-License-Identifier: Apache-2.0
//
// MyLinux boot protocol scaffold
// Target: Samsung SM-T295 (gto), ARM64
//
// Experimental inspection only. Not a functioning bootloader.

#include <sys/auxv.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <fstream>

namespace mylinux_boot {

constexpr size_t HEADER_SIZE = 64;
constexpr size_t MAGIC_OFFSET = 0x38;
constexpr uint32_t ARM64_MAGIC = 0x644D5241;

struct ImageInfo {
    uint64_t text_offset = 0;
    uint64_t image_size = 0;
    uint64_t flags = 0;
    bool valid_magic = false;
};

static uint64_t read_le64(
    const unsigned char* data,
    size_t offset
) {
    uint64_t value = 0;

    for (unsigned int i = 0; i < 8; ++i) {
        value |= uint64_t(data[offset + i]) << (i * 8);
    }

    return value;
}

// Inspect an uncompressed ARM64 Linux Image.
// Does not execute or load the image.
bool inspect_image(const char* path, ImageInfo& info) {
    if (path == nullptr || *path == '\0') {
        fprintf(stderr, "Error: missing image path\n");
        return false;
    }

    std::ifstream file(path, std::ios::binary);

    if (!file) {
        fprintf(stderr, "Error: cannot open image\n");
        return false;
    }

    unsigned char header[HEADER_SIZE] = {};

    file.read(
        reinterpret_cast<char*>(header),
        HEADER_SIZE
    );

    if (file.gcount() != HEADER_SIZE) {
        fprintf(stderr, "Error: incomplete image header\n");
        return false;
    }

    const uint32_t magic =
        uint32_t(header[MAGIC_OFFSET]) |
        (uint32_t(header[MAGIC_OFFSET + 1]) << 8) |
        (uint32_t(header[MAGIC_OFFSET + 2]) << 16) |
        (uint32_t(header[MAGIC_OFFSET + 3]) << 24);

    info.text_offset = read_le64(header, 0x08);
    info.image_size  = read_le64(header, 0x10);
    info.flags       = read_le64(header, 0x18);
    info.valid_magic = (magic == ARM64_MAGIC);

    std::cout << std::hex;
    std::cout << "text_offset: 0x" << info.text_offset << '\n';
    std::cout << "image_size:  0x" << info.image_size << '\n';
    std::cout << "flags:       0x" << info.flags << '\n';
    std::cout << "ARM64 magic: "
              << (info.valid_magic ? "matched" : "not matched")
              << '\n';

    return true;
}

} // namespace mylinux_boot

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Image>\n", argv[0]);
        return 2;
    }

    mylinux_boot::ImageInfo info;

    if (!mylinux_boot::inspect_image(argv[1], info)) {
        return 1;
    }

    return info.valid_magic ? 0 : 1;
}
