#include "manager_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

static char *trim(char *text)
{
    char *end;

    while (*text && isspace((unsigned char)*text)) {
        text++;
    }

    end = text + strlen(text);

    while (end > text && isspace((unsigned char)end[-1])) {
        end--;
    }

    *end = '\0';
    return text;
}

static int copy_value(
    char *destination,
    size_t capacity,
    const char *value)
{
    size_t length;

    if (!destination || !value || capacity == 0) {
        return -1;
    }

    length = strlen(value);

    if (length >= capacity) {
        return -1;
    }

    memcpy(destination, value, length + 1);
    return 0;
}

static int parse_bool(const char *value, int *result)
{
    if (!value || !result) {
        return -1;
    }

    if (strcmp(value, "yes") == 0 ||
        strcmp(value, "true") == 0 ||
        strcmp(value, "1") == 0) {
        *result = 1;
        return 0;
    }

    if (strcmp(value, "no") == 0 ||
        strcmp(value, "false") == 0 ||
        strcmp(value, "0") == 0) {
        *result = 0;
        return 0;
    }

    return -1;
}

static int parse_uint32(const char *value, uint32_t *result)
{
    char *end;
    unsigned long parsed;

    if (!value || !result || *value == '\0' || *value == '-') {
        return -1;
    }

    errno = 0;
    parsed = strtoul(value, &end, 10);

    if (errno != 0 || *end != '\0' ||
        parsed > UINT32_MAX) {
        return -1;
    }

    *result = (uint32_t)parsed;
    return 0;
}

static int assign_value(
    manager_config_t *c,
    const char *section,
    const char *key,
    const char *value)
{
#define COPY_FIELD(k, field) \
    if (strcmp(key, k) == 0) \
        return copy_value(c->field, sizeof(c->field), value)

#define BOOL_FIELD(k, field) \
    if (strcmp(key, k) == 0) \
        return parse_bool(value, &c->field)

#define UINT_FIELD(k, field) \
    if (strcmp(key, k) == 0) \
        return parse_uint32(value, &c->field)

    if (strcmp(section, "Manager") == 0) {
        COPY_FIELD("Name", name);
        COPY_FIELD("Version", version);
        COPY_FIELD("Mode", mode);
    } else if (strcmp(section, "Logging") == 0) {
        COPY_FIELD("Level", log_level);
        COPY_FIELD("Output", log_output);
        BOOL_FIELD("IncludeTimestamp", include_timestamp);
        BOOL_FIELD("IncludeProcessId", include_process_id);
    } else if (strcmp(section, "Process") == 0) {
        UINT_FIELD("MaxTrackedProcesses", max_tracked_processes);
        COPY_FIELD("DefaultRestartPolicy", restart_policy);
        UINT_FIELD("RestartDelayMs", restart_delay_ms);
        UINT_FIELD("ShutdownTimeoutSec", shutdown_timeout_sec);
    } else if (strcmp(section, "Resources") == 0) {
        UINT_FIELD("MaxOpenFiles", max_open_files);
        UINT_FIELD("MaxServices", max_services);
        UINT_FIELD("MaxDependencies", max_dependencies);
    } else if (strcmp(section, "Security") == 0) {
        BOOL_FIELD("AllowShellCommands", allow_shell_commands);
        BOOL_FIELD("AllowUnknownServices", allow_unknown_services);
        BOOL_FIELD("RequireAbsoluteExecutablePath",
                   require_absolute_executable_path);
    } else if (strcmp(section, "Runtime") == 0) {
        COPY_FIELD("StateDirectory", state_directory);
        COPY_FIELD("ConfigurationDirectory", configuration_directory);
        COPY_FIELD("ServiceDirectory", service_directory);
    } else if (strcmp(section, "Compatibility") == 0) {
        BOOL_FIELD("AndroidInitRcSupport",
                   android_init_rc_metadata_only);
        BOOL_FIELD("SystemdUnitSupport", systemd_unit_support_planned);
    } else if (strcmp(section, "Status") == 0) {
        BOOL_FIELD("Complete", complete);
        BOOL_FIELD("StartsServices", starts_services);
        BOOL_FIELD("ExecutesCommands", executes_commands);
    }

#undef COPY_FIELD
#undef BOOL_FIELD
#undef UINT_FIELD

    return -2;
}

int manager_config_load(
    const char *path,
    manager_config_t *config)
{
    FILE *file;
    char line[MANAGER_CONFIG_LINE_MAX];
    char section[64] = "";
    unsigned int line_number = 0;

    if (!path || !config) {
        return -1;
    }

    memset(config, 0, sizeof(*config));

    file = fopen(path, "r");

    if (!file) {
        return -2;
    }

    while (fgets(line, sizeof(line), file)) {
        char *text;
        char *equals;
        size_t length;

        line_number++;
        length = strlen(line);

        if (length > 0 && line[length - 1] != '\n' &&
            !feof(file)) {
            fclose(file);
            return -3;
        }

        text = trim(line);

        if (*text == '\0' || *text == '#' || *text == ';') {
            continue;
        }

        if (*text == '[') {
            char *closing = strchr(text, ']');

            if (!closing || closing[1] != '\0') {
                fclose(file);
                return -4;
            }

            *closing = '\0';

            if (copy_value(section, sizeof(section), text + 1) != 0) {
                fclose(file);
                return -5;
            }

            continue;
        }

        equals = strchr(text, '=');

        if (!equals || section[0] == '\0') {
            fclose(file);
            return -6;
        }

        *equals = '\0';

        {
            char *key = trim(text);
            char *value = trim(equals + 1);
            int result = assign_value(config, section, key, value);

            if (result != 0) {
                fprintf(stderr,
                        "manager_config: invalid setting at line %u\n",
                        line_number);
                fclose(file);
                return -7;
            }
        }
    }

    if (ferror(file)) {
        fclose(file);
        return -8;
    }

    fclose(file);
    return manager_config_validate(config);
}

int manager_config_validate(const manager_config_t *c)
{
    if (!c) {
        return -1;
    }

    if (c->name[0] == '\0' ||
        c->version[0] == '\0' ||
        c->mode[0] == '\0') {
        return -2;
    }

    if (c->max_tracked_processes == 0 ||
        c->max_services == 0 ||
        c->max_open_files == 0) {
        return -3;
    }

    if (c->state_directory[0] != '/' ||
        c->configuration_directory[0] != '/' ||
        c->service_directory[0] != '/') {
        return -4;
    }

    /*
     * This prototype intentionally refuses to claim it can run services.
     */
    if (c->starts_services || c->executes_commands) {
        return -5;
    }

    if (c->allow_shell_commands || c->allow_unknown_services) {
        return -6;
    }

    return 0;
}
