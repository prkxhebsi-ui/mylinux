#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/mylinux/runtime.h"

#define MYLINUX_RUNTIME_DIR_MODE 0755

static int create_directory(const char *path, mode_t mode)
{
    struct stat info;

    if (path == NULL || path[0] != '/') {
        errno = EINVAL;
        return -1;
    }

    /*
     * This checks the final path component without following a symlink.
     * Parent components must also be trusted and protected from modification.
     */
    if (lstat(path, &info) == 0) {
        if (S_ISLNK(info.st_mode) || !S_ISDIR(info.st_mode)) {
            errno = EEXIST;
            return -1;
        }

        if (chmod(path, mode) != 0) {
            return -1;
        }

        return 0;
    }

    if (errno != ENOENT) {
        return -1;
    }

    if (mkdir(path, mode) != 0) {
        return -1;
    }

    if (lstat(path, &info) != 0 ||
        S_ISLNK(info.st_mode) ||
        !S_ISDIR(info.st_mode)) {
        errno = EINVAL;
        return -1;
    }

    return chmod(path, mode);
}

static int initialize_one(const char *path)
{
    if (create_directory(path, MYLINUX_RUNTIME_DIR_MODE) != 0) {
        fprintf(stderr, "runtime: cannot prepare %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    printf("runtime: ready: %s\n", path);
    return 0;
}

int mylinux_runtime_initialize(mylinux_runtime_info_t *runtime)
{
    if (runtime == NULL) {
        errno = EINVAL;
        return -1;
    }

    runtime->state = MYLINUX_RUNTIME_INITIALIZING;

    if (initialize_one(runtime->root) != 0 ||
        initialize_one(runtime->pid_directory) != 0 ||
        initialize_one(runtime->socket_directory) != 0 ||
        initialize_one(runtime->lock_directory) != 0 ||
        initialize_one(runtime->journal_directory) != 0) {
        runtime->state = MYLINUX_RUNTIME_FAILED;
        runtime->last_error = MYLINUX_STATUS_ERROR;
        return -1;
    }

    runtime->initialized_at = time(NULL);
    runtime->state = MYLINUX_RUNTIME_READY;
    runtime->last_error = MYLINUX_STATUS_OK;

    return 0;
}

#ifdef MYLINUX_RUNTIME_STANDALONE
int main(void)
{
    mylinux_runtime_info_t runtime;

    if (mylinux_runtime_info_init(&runtime) != MYLINUX_STATUS_OK) {
        fprintf(stderr, "runtime: failed to initialize runtime metadata\n");
        return 1;
    }

    if (mylinux_runtime_initialize(&runtime) != 0) {
        return 1;
    }

    printf("runtime: initialization complete\n");
    return 0;
}
#endif
