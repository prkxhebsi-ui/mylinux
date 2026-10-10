#ifndef MYLINUX_MANAGER_CONFIG_H
#define MYLINUX_MANAGER_CONFIG_H

#include <stddef.h>
#include <stdint.h>
#include "../include/mylinux/types.h"

#define MANAGER_CONFIG_PATH_MAX 512
#define MANAGER_CONFIG_LINE_MAX 1024

typedef struct {
    char name[128];
    char version[32];
    char mode[32];

    char log_level[32];
    char log_output[32];
    int include_timestamp;
    int include_process_id;

    uint32_t max_tracked_processes;
    char restart_policy[32];
    uint32_t restart_delay_ms;
    uint32_t shutdown_timeout_sec;

    uint32_t max_open_files;
    uint32_t max_services;
    uint32_t max_dependencies;

    int allow_shell_commands;
    int allow_unknown_services;
    int require_absolute_executable_path;

    char state_directory[MANAGER_CONFIG_PATH_MAX];
    char configuration_directory[MANAGER_CONFIG_PATH_MAX];
    char service_directory[MANAGER_CONFIG_PATH_MAX];

    int android_init_rc_metadata_only;
    int systemd_unit_support_planned;

    int complete;
    int starts_services;
    int executes_commands;
} manager_config_t;

int manager_config_load(
    const char *path,
    manager_config_t *config);

int manager_config_validate(
    const manager_config_t *config);

#endif
