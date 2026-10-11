#ifndef MYLINUX_RUNTIME_H
#define MYLINUX_RUNTIME_H

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include "types.h"
#include "mylinux_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYLINUX_RUNTIME_ROOT       "/run/mylinux"
#define MYLINUX_RUNTIME_PID_DIR    "/run/mylinux/pids"
#define MYLINUX_RUNTIME_SOCKET_DIR "/run/mylinux/sockets"
#define MYLINUX_RUNTIME_LOCK_DIR   "/run/mylinux/locks"
#define MYLINUX_RUNTIME_JOURNAL    "/run/mylinux/journal"

#define MYLINUX_RUNTIME_PATH_MAX 512
#define MYLINUX_RUNTIME_NAME_MAX 128

typedef enum {
    MYLINUX_RUNTIME_UNINITIALIZED = 0,
    MYLINUX_RUNTIME_INITIALIZING,
    MYLINUX_RUNTIME_READY,
    MYLINUX_RUNTIME_FAILED,
    MYLINUX_RUNTIME_STOPPING
} mylinux_runtime_state_t;

typedef struct {
    char root[MYLINUX_RUNTIME_PATH_MAX];
    char pid_directory[MYLINUX_RUNTIME_PATH_MAX];
    char socket_directory[MYLINUX_RUNTIME_PATH_MAX];
    char lock_directory[MYLINUX_RUNTIME_PATH_MAX];
    char journal_directory[MYLINUX_RUNTIME_PATH_MAX];

    mylinux_runtime_state_t state;
    mylinux_time initialized_at;
    mylinux_status_t last_error;
} mylinux_runtime_info_t;

typedef struct {
    char name[MYLINUX_RUNTIME_NAME_MAX];
    char path[MYLINUX_RUNTIME_PATH_MAX];
    pid_t owner_pid;
    uint32_t mode;
    int registered;
} mylinux_runtime_object_t;

static inline int
mylinux_runtime_copy_path(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL ||
        source == NULL ||
        capacity == 0) {
        return MYLINUX_STATUS_INVALID;
    }

    length = strlen(source);

    if (length >= capacity) {
        return MYLINUX_STATUS_INVALID;
    }

    memcpy(destination, source, length + 1);
    return MYLINUX_STATUS_OK;
}

static inline int
mylinux_runtime_info_init(mylinux_runtime_info_t *runtime)
{
    if (runtime == NULL) {
        return MYLINUX_STATUS_INVALID;
    }

    memset(runtime, 0, sizeof(*runtime));

    if (mylinux_runtime_copy_path(
            runtime->root, sizeof(runtime->root),
            MYLINUX_RUNTIME_ROOT) != MYLINUX_STATUS_OK) {
        return MYLINUX_STATUS_ERROR;
    }

    if (mylinux_runtime_copy_path(
            runtime->pid_directory, sizeof(runtime->pid_directory),
            MYLINUX_RUNTIME_PID_DIR) != MYLINUX_STATUS_OK) {
        return MYLINUX_STATUS_ERROR;
    }

    if (mylinux_runtime_copy_path(
            runtime->socket_directory,
            sizeof(runtime->socket_directory),
            MYLINUX_RUNTIME_SOCKET_DIR) != MYLINUX_STATUS_OK) {
        return MYLINUX_STATUS_ERROR;
    }

    if (mylinux_runtime_copy_path(
            runtime->lock_directory, sizeof(runtime->lock_directory),
            MYLINUX_RUNTIME_LOCK_DIR) != MYLINUX_STATUS_OK) {
        return MYLINUX_STATUS_ERROR;
    }

    if (mylinux_runtime_copy_path(
            runtime->journal_directory,
            sizeof(runtime->journal_directory),
            MYLINUX_RUNTIME_JOURNAL) != MYLINUX_STATUS_OK) {
        return MYLINUX_STATUS_ERROR;
    }

    runtime->state = MYLINUX_RUNTIME_INITIALIZING;
    runtime->last_error = MYLINUX_STATUS_OK;

    return MYLINUX_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_RUNTIME_H */
