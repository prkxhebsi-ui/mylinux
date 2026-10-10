#ifndef MYLINUX_TYPES_H #define MYLINUX_TYPES_H

#include <stdbool.h> #include <stddef.h> #include <stdint.h>

/* MyLinux project-level types.

These are not replacements for kernel or libc types. */ 

typedef enum { MYLINUX_STATUS_OK = 0, MYLINUX_STATUS_ERROR = 1, MYLINUX_STATUS_INVALID_ARGUMENT = 2, MYLINUX_STATUS_NOT_SUPPORTED = 3 } mylinux_status_t;

typedef struct { mylinux_status_t status; const char *message; } mylinux_result_t;

#endif /* MYLINUX_TYPES_H */

