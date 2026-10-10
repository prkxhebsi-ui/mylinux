#ifndef MYLINUX_STATUS_H #define MYLINUX_STATUS_H

#include "mylinux_types.h"

#ifdef __cplusplus extern "C" { #endif

typedef enum { MYLINUX_BOOT_NOT_STARTED = 0, MYLINUX_BOOT_INITIALIZING, MYLINUX_BOOT_READY, MYLINUX_BOOT_FAILED, MYLINUX_BOOT_STOPPING, MYLINUX_BOOT_STOPPED } mylinux_boot_state_t;

typedef struct { mylinux_boot_state_t state; int error_code; const char *error_message; } mylinux_boot_status_t;

mylinux_status_t mylinux_status_init(mylinux_boot_status_t *status); mylinux_status_t mylinux_status_set( mylinux_boot_status_t *status, mylinux_boot_state_t state, int error_code, const char *error_message );

#ifdef __cplusplus } #endif

#endif /* MYLINUX_STATUS_H */

