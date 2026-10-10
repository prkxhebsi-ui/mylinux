#include "manager_config.h"

#include <stdio.h>
#include <string.h>

static void print_config(const manager_config_t *c)
{
    printf("MyLinux Manager Configuration\n");
    printf("Name: %s\n", c->name);
    printf("Version: %s\n", c->version);
    printf("Mode: %s\n", c->mode);

    printf("\n[Logging]\n");
    printf("Level: %s\n", c->log_level);
    printf("Output: %s\n", c->log_output);
    printf("Timestamp: %s\n",
           c->include_timestamp ? "yes" : "no");
    printf("Process ID: %s\n",
           c->include_process_id ? "yes" : "no");

    printf("\n[Process]\n");
    printf("Tracked processes: %u\n",
           c->max_tracked_processes);
    printf("Restart policy: %s\n", c->restart_policy);
    printf("Restart delay: %u ms\n", c->restart_delay_ms);
    printf("Shutdown timeout: %u seconds\n",
           c->shutdown_timeout_sec);

    printf("\n[Resources]\n");
    printf("Maximum open files: %u\n", c->max_open_files);
    printf("Maximum services: %u\n", c->max_services);
    printf("Maximum dependencies: %u\n", c->max_dependencies);

    printf("\n[Runtime]\n");
    printf("State: %s\n", c->state_directory);
    printf("Configuration: %s\n",
           c->configuration_directory);
    printf("Services: %s\n", c->service_directory);

    printf("\n[Security]\n");
    printf("Shell commands: %s\n",
           c->allow_shell_commands ? "allowed" : "disabled");
    printf("Unknown services: %s\n",
           c->allow_unknown_services ? "allowed" : "disabled");
    printf("Absolute executable paths required: %s\n",
           c->require_absolute_executable_path ? "yes" : "no");

    printf("\n[Status]\n");
    printf("Complete: %s\n", c->complete ? "yes" : "no");
    printf("Starts services: %s\n",
           c->starts_services ? "yes" : "no");
    printf("Executes commands: %s\n",
           c->executes_commands ? "yes" : "no");
}

int main(int argc, char **argv)
{
    manager_config_t config;
    const char *path = "systemd/managerd.conf";
    int result;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [managerd.conf]\n", argv[0]);
        return 2;
    }

    if (argc == 2) {
        path = argv[1];
    }

    result = manager_config_load(path, &config);

    if (result != 0) {
        fprintf(stderr,
                "Failed to load or validate configuration (%d).\n",
                result);
        return 1;
    }

    print_config(&config);

    printf("\nConfiguration parsed successfully.\n");
    printf("Runner mode: validation only; no services launched.\n");

    return 0;
}
