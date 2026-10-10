#ifndef MYLINUX_COMPILER_H
#define MYLINUX_COMPILER_H

/* Compiler feature detection for MyLinux source code. */

#if defined(clang)
#define MYLINUX_COMPILER_CLANG 1
#define MYLINUX_COMPILER_NAME "Clang"
#define MYLINUX_COMPILER_VERSION 
(clang_major * 10000 + clang_minor * 100 + clang_patchlevel)
#elif defined(GNUC)
#define MYLINUX_COMPILER_GCC 1
#define MYLINUX_COMPILER_NAME "GCC"
#define MYLINUX_COMPILER_VERSION 
(GNUC * 10000 + GNUC_MINOR * 100 + GNUC_PATCHLEVEL)
#else
#define MYLINUX_COMPILER_NAME "Unknown"
#define MYLINUX_COMPILER_VERSION 0
#endif

#if defined(__cplusplus)
#define MYLINUX_LANGUAGE_CPP 1
#define MYLINUX_LANGUAGE_C 0
#else
#define MYLINUX_LANGUAGE_CPP 0
#define MYLINUX_LANGUAGE_C 1
#endif

#if defined(__has_attribute)
#define MYLINUX_HAS_ATTRIBUTE(x) __has_attribute(x)
#else
#define MYLINUX_HAS_ATTRIBUTE(x) 0
#endif

#if defined(__has_builtin)
#define MYLINUX_HAS_BUILTIN(x) __has_builtin(x)
#else
#define MYLINUX_HAS_BUILTIN(x) 0
#endif

#if defined(__has_feature)
#define MYLINUX_HAS_FEATURE(x) __has_feature(x)
#else
#define MYLINUX_HAS_FEATURE(x) 0
#endif

#endif /* MYLINUX_COMPILER_H */

