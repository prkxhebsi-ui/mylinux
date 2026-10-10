#ifndef MYLINUX_PROCESS_H
#define MYLINUX_PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <time.h>

#include "types.h"
#include "mylinux_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYLINUX_PROCESS_NAME_MAX 128
#define MYLINUX_PROCESS_INVALID_ID ((pid_t)-1)

typedef enum {
    MYLINUX_PROCESS_NEW = 0,
    MYLINUX_PROCESS_STARTING,
    MYLINUX_PROCESS_RUNNING,
    MYLINUX_PROCESS_STOPPING,
    MYLINUX_PROCESS_EXITED,
    MYLINUX_PROCESS_FAILED
} mylinux_process_state_t;

typedef struct {
    pid_t pid;
    pid_t parent_pid;
    mylinux_process_state_t state;
    int exit_status;
    int signal_number;
    mylinux_time started_at;
    mylinux_time exited_at;
    char name[MYLINUX_PROCESS_NAME_MAX];
} mylinux_process_info_t;

typedef struct {
    pid_t pid;
    int status;
    int exited;
    int terminated_by_signal;
    int signal_number;
} mylinux_wait_result_t;

/*
 * Initialize process metadata.
 * This function does not create or launch a process.
 */
static inline int
mylinux_process_info_init(
    mylinux_process_info_t *process,
    const char *name)
{
    if (process == NULL) {
        return MYLINUX_STATUS_INVALID;
    }

    process->pid = MYLINUX_PROCESS_INVALID_ID;
    process->parent_pid = MYLINUX_PROCESS_INVALID_ID;
    process->state = MYLINUX_PROCESS_NEW;
    process->exit_status = 0;
    process->signal_number = 0;
    process->started_at = (mylinux_time)0;
    process->exited_at = (mylinux_time)0;
    process->name[0] = '\0';

    if (name != NULL) {
        size_t index = 0;

        while (index < MYLINUX_PROCESS_NAME_MAX - 1 &&
               name[index] != '\0') {
            process->name[index] = name[index];
            index++;
        }

        process->name[index] = '\0';
    }

    return MYLINUX_STATUS_OK;
}

/*
 * Record a waitpid() result.
 * Pass the status returned through waitpid().
 */
static inline int
mylinux_process_decode_wait_status(
    int wait_status,
    mylinux_wait_result_t *result)
{
    if (result == NULL) {
        return MYLINUX_STATUS_INVALID;
    }

    result->status = wait_status;
    result->exited = 0;
    result->terminated_by_signal = 0;
    result->signal_number = 0;

    if (WIFEXITED(wait_status)) {
        result->exited = 1;
        result->status = WEXITSTATUS(wait_status);
    } else if (WIFSIGNALED(wait_status)) {
        result->terminated_by_signal = 1;
        result->signal_number = WTERMSIG(wait_status);
    }

    return MYLINUX_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* MYLINUX_PROCESS_H */
