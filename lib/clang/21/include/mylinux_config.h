#ifndef MYLINUX_CONFIG_H #define MYLINUX_CONFIG_H

/* MyLinux build-time configuration.

Override defaults through compiler -D options when appropriate. */ 

#ifndef MYLINUX_VERSION_MAJOR #define MYLINUX_VERSION_MAJOR 0 #endif

#ifndef MYLINUX_VERSION_MINOR #define MYLINUX_VERSION_MINOR 1 #endif

#ifndef MYLINUX_VERSION_PATCH #define MYLINUX_VERSION_PATCH 0 #endif

#ifndef MYLINUX_ARCH_AARCH64 #define MYLINUX_ARCH_AARCH64 1 #endif

#ifndef MYLINUX_ENABLE_RC_PARSER #define MYLINUX_ENABLE_RC_PARSER 1 #endif

#ifndef MYLINUX_ENABLE_MOUNT_CONFIG #define MYLINUX_ENABLE_MOUNT_CONFIG 1 #endif

#ifndef MYLINUX_ENABLE_BP_PARSER #define MYLINUX_ENABLE_BP_PARSER 1 #endif

#define MYLINUX_VERSION_STRING "0.1.0"

#endif /* MYLINUX_CONFIG_H */

