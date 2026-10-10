/*
 * MyLinux .rc Line Runner
 *
 * Prototype for processing a limited subset of Android-style init
 * configuration lines.
 *
 * Safety:
 *   - Does not invoke a shell.
 *   - Does not execute arbitrary programs.
 *   - Does not mount filesystems or change device permissions.
 *   - Unknown commands are rejected.
 *
 * Build as a userspace test program, not as kernel code.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#define RC_RUNNER_MAX_LINE       1024
#define RC_RUNNER_MAX_TOKEN       128
#define RC_RUNNER_MAX_TOKENS       32
#define RC_RUNNER_MAX_ACTIONS     128
#define RC_RUNNER_MAX_SERVICES     64
#define RC_RUNNER_MAX_IMPORTS      64
#define RC_RUNNER_MAX_PATH        512

typedef enum {
    RC_RUN_OK = 0,
    RC_RUN_INVALID_ARGUMENT,
    RC_RUN_LINE_TOO_LONG,
    RC_RUN_TOO_MANY_TOKENS,
    RC_RUN_UNKNOWN_COMMAND,
    RC_RUN_UNSUPPORTED_OPERATION,
    RC_RUN_LIMIT_REACHED,
    RC_RUN_IO_ERROR
} rc_run_status_t;

typedef enum {
    RC_RUNNER_IDLE = 0,
    RC_RUNNER_IN_ACTION,
    RC_RUNNER_STOPPED,
    RC_RUNNER_FAILED
} rc_runner_state_t;

typedef struct {
    unsigned int line_number;
    char trigger[RC_RUNNER_MAX_TOKEN];
    unsigned int command_count;
} rc_action_t;

typedef struct {
    char name[RC_RUNNER_MAX_TOKEN];
    char executable[RC_RUNNER_MAX_PATH];
    int declared;
} rc_service_t;

typedef struct {
    char path[RC_RUNNER_MAX_PATH];
} rc_import_t;

typedef struct {
    rc_runner_state_t state;

    rc_action_t actions[RC_RUNNER_MAX_ACTIONS];
    size_t action_count;

    rc_service_t services[RC_RUNNER_MAX_SERVICES];
    size_t service_count;

    rc_import_t imports[RC_RUNNER_MAX_IMPORTS];
    size_t import_count;

    unsigned int lines_processed;
    unsigned int commands_accepted;
    unsigned int commands_rejected;
} rc_runner_t;

typedef struct {
    char storage[RC_RUNNER_MAX_LINE];
    char *tokens[RC_RUNNER_MAX_TOKENS];
    size_t count;
} rc_tokens_t;

static void rc_runner_trim(char *text)
{
    size_t length;
    size_t start = 0;

    if (text == NULL) {
        return;
    }

    length = strlen(text);

    while (start < length &&
           isspace((unsigned char)text[start])) {
        start++;
    }

    if (start > 0) {
        memmove(text, text + start, length - start + 1);
        length -= start;
    }

    while (length > 0 &&
           isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
}

static int rc_runner_copy(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL ||
        source == NULL ||
        capacity == 0) {
        return -1;
    }

    length = strlen(source);

    if (length >= capacity) {
        return -1;
    }

    memcpy(destination, source, length + 1);
    return 0;
}

static void rc_runner_init(rc_runner_t *runner)
{
    if (runner == NULL) {
        return;
    }

    memset(runner, 0, sizeof(*runner));
    runner->state = RC_RUNNER_IDLE;
}

static int rc_runner_tokenize(
    const char *line,
    rc_tokens_t *result)
{
    size_t input_length;
    char *cursor;

    if (line == NULL || result == NULL) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    input_length = strlen(line);

    if (input_length >= sizeof(result->storage)) {
        return RC_RUN_LINE_TOO_LONG;
    }

    memset(result, 0, sizeof(*result));
    memcpy(result->storage, line, input_length + 1);

    cursor = result->storage;

    while (*cursor != '\0') {
        char *start;

        while (*cursor != '\0' &&
               isspace((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor == '\0' || *cursor == '#') {
            break;
        }

        if (result->count >= RC_RUNNER_MAX_TOKENS) {
            return RC_RUN_TOO_MANY_TOKENS;
        }

        start = cursor;

        while (*cursor != '\0' &&
               !isspace((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor != '\0') {
            *cursor++ = '\0';
        }

        if (strlen(start) >= RC_RUNNER_MAX_TOKEN) {
            return RC_RUN_LINE_TOO_LONG;
        }

        result->tokens[result->count++] = start;
    }

    return RC_RUN_OK;
}

static int rc_runner_add_import(
    rc_runner_t *runner,
    const char *path)
{
    size_t i;

    if (runner == NULL || path == NULL || path[0] == '\0') {
        return RC_RUN_INVALID_ARGUMENT;
    }

    for (i = 0; i < runner->import_count; i++) {
        if (strcmp(runner->imports[i].path, path) == 0) {
            return RC_RUN_OK;
        }
    }

    if (runner->import_count >= RC_RUNNER_MAX_IMPORTS) {
        return RC_RUN_LIMIT_REACHED;
    }

    if (rc_runner_copy(
            runner->imports[runner->import_count].path,
            sizeof(runner->imports[0].path),
            path) != 0) {
        return RC_RUN_LINE_TOO_LONG;
    }

    runner->import_count++;
    return RC_RUN_OK;
}

static int rc_runner_add_service(
    rc_runner_t *runner,
    const char *name,
    const char *executable)
{
    size_t i;

    if (runner == NULL || name == NULL || executable == NULL) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    for (i = 0; i < runner->service_count; i++) {
        if (strcmp(runner->services[i].name, name) == 0) {
            return RC_RUN_INVALID_ARGUMENT;
        }
    }

    if (runner->service_count >= RC_RUNNER_MAX_SERVICES) {
        return RC_RUN_LIMIT_REACHED;
    }

    rc_service_t *service = &runner->services[runner->service_count];

    if (rc_runner_copy(service->name,
                       sizeof(service->name), name) != 0 ||
        rc_runner_copy(service->executable,
                       sizeof(service->executable), executable) != 0) {
        return RC_RUN_LINE_TOO_LONG;
    }

    service->declared = 1;
    runner->service_count++;
    return RC_RUN_OK;
}

static int rc_runner_add_action(
    rc_runner_t *runner,
    unsigned int line_number,
    const char *trigger)
{
    rc_action_t *action;

    if (runner == NULL || trigger == NULL) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    if (runner->action_count >= RC_RUNNER_MAX_ACTIONS) {
        return RC_RUN_LIMIT_REACHED;
    }

    action = &runner->actions[runner->action_count];

    if (rc_runner_copy(action->trigger,
                       sizeof(action->trigger), trigger) != 0) {
        return RC_RUN_LINE_TOO_LONG;
    }

    action->line_number = line_number;
    action->command_count = 0;

    runner->action_count++;
    runner->state = RC_RUNNER_IN_ACTION;

    return RC_RUN_OK;
}

static int rc_runner_safe_command(
    rc_runner_t *runner,
    const rc_tokens_t *tokens)
{
    const char *command;

    if (runner == NULL || tokens == NULL || tokens->count == 0) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    command = tokens->tokens[0];

    /*
     * These operations are deliberately metadata-only.
     * They do not change the real system state.
     */
    if (strcmp(command, "loglevel") == 0) {
        if (tokens->count != 2) {
            return RC_RUN_INVALID_ARGUMENT;
        }

        if (strcmp(tokens->tokens[1], "debug") != 0 &&
            strcmp(tokens->tokens[1], "info") != 0 &&
            strcmp(tokens->tokens[1], "warning") != 0 &&
            strcmp(tokens->tokens[1], "error") != 0) {
            return RC_RUN_INVALID_ARGUMENT;
        }

        printf("rc-runner: accepted loglevel %s\n",
               tokens->tokens[1]);
        return RC_RUN_OK;
    }

    if (strcmp(command, "setprop") == 0) {
        if (tokens->count != 3) {
            return RC_RUN_INVALID_ARGUMENT;
        }

        printf("rc-runner: recorded property request %s=%s\n",
               tokens->tokens[1], tokens->tokens[2]);
        return RC_RUN_OK;
    }

    if (strcmp(command, "write") == 0) {
        /*
         * Never write arbitrary paths from a config file.
         */
        return RC_RUN_UNSUPPORTED_OPERATION;
    }

    if (strcmp(command, "mount") == 0 ||
        strcmp(command, "umount") == 0 ||
        strcmp(command, "chmod") == 0 ||
        strcmp(command, "chown") == 0 ||
        strcmp(command, "exec") == 0 ||
        strcmp(command, "exec_start") == 0 ||
        strcmp(command, "start") == 0 ||
        strcmp(command, "stop") == 0 ||
        strcmp(command, "restart") == 0) {
        return RC_RUN_UNSUPPORTED_OPERATION;
    }

    return RC_RUN_UNKNOWN_COMMAND;
}

static int rc_runner_process_line(
    rc_runner_t *runner,
    const char *line,
    unsigned int line_number)
{
    rc_tokens_t tokens;
    int status;

    if (runner == NULL || line == NULL) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    status = rc_runner_tokenize(line, &tokens);

    if (status != RC_RUN_OK) {
        runner->commands_rejected++;
        return status;
    }

    runner->lines_processed++;

    if (tokens.count == 0) {
        return RC_RUN_OK;
    }

    if (strcmp(tokens.tokens[0], "on") == 0) {
        if (tokens.count < 2) {
            runner->commands_rejected++;
            return RC_RUN_INVALID_ARGUMENT;
        }

        status = rc_runner_add_action(
            runner, line_number, tokens.tokens[1]);

        if (status == RC_RUN_OK) {
            printf("rc-runner: action trigger '%s'\n",
                   tokens.tokens[1]);
        }

        return status;
    }

    if (strcmp(tokens.tokens[0], "import") == 0) {
        if (tokens.count != 2) {
            runner->commands_rejected++;
            return RC_RUN_INVALID_ARGUMENT;
        }

        status = rc_runner_add_import(runner, tokens.tokens[1]);

        if (status == RC_RUN_OK) {
            printf("rc-runner: recorded import '%s'\n",
                   tokens.tokens[1]);
        }

        return status;
    }

    if (strcmp(tokens.tokens[0], "service") == 0) {
        if (tokens.count < 3) {
            runner->commands_rejected++;
            return RC_RUN_INVALID_ARGUMENT;
        }

        status = rc_runner_add_service(
            runner, tokens.tokens[1], tokens.tokens[2]);

        if (status == RC_RUN_OK) {
            printf("rc-runner: declared service '%s'\n",
                   tokens.tokens[1]);
        }

        return status;
    }

    if (runner->state != RC_RUNNER_IN_ACTION) {
        runner->commands_rejected++;
        return RC_RUN_UNSUPPORTED_OPERATION;
    }

    status = rc_runner_safe_command(runner, &tokens);

    if (status == RC_RUN_OK) {
        runner->commands_accepted++;

        if (runner->action_count > 0) {
            runner->actions[runner->action_count - 1].command_count++;
        }
    } else {
        runner->commands_rejected++;
        fprintf(stderr,
                "rc-runner: rejected line %u (status %d)\n",
                line_number, status);
    }

    return status;
}

static int rc_runner_process_stream(
    rc_runner_t *runner,
    FILE *input)
{
    char line[RC_RUNNER_MAX_LINE];
    unsigned int line_number = 0;
    int errors = 0;

    if (runner == NULL || input == NULL) {
        return RC_RUN_INVALID_ARGUMENT;
    }

    while (fgets(line, sizeof(line), input) != NULL) {
        int status;

        line_number++;

        if (strchr(line, '\n') == NULL && !feof(input)) {
            int ch;

            while ((ch = fgetc(input)) != '\n' && ch != EOF) {
                /* Discard remainder of overlong line. */
            }

            fprintf(stderr,
                    "rc-runner: line %u exceeds maximum length\n",
                    line_number);
            runner->commands_rejected++;
            errors++;
            continue;
        }

        status = rc_runner_process_line(runner, line, line_number);

        if (status != RC_RUN_OK) {
            errors++;
        }
    }

    if (ferror(input)) {
        runner->state = RC_RUNNER_FAILED;
        return RC_RUN_IO_ERROR;
    }

    runner->state = RC_RUNNER_STOPPED;
    return errors;
}

static void rc_runner_print_summary(const rc_runner_t *runner)
{
    if (runner == NULL) {
        return;
    }

    printf("\nMyLinux RC runner summary\n");
    printf("Lines processed: %u\n", runner->lines_processed);
    printf("Actions declared: %zu\n", runner->action_count);
    printf("Services declared: %zu\n", runner->service_count);
    printf("Imports recorded: %zu\n", runner->import_count);
    printf("Commands accepted: %u\n", runner->commands_accepted);
    printf("Commands rejected: %u\n", runner->commands_rejected);
}

int main(int argc, char **argv)
{
    FILE *input;
    rc_runner_t runner;
    int result;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.rc\n", argv[0]);
        return 2;
    }

    input = fopen(argv[1], "r");

    if (input == NULL) {
        fprintf(stderr, "Cannot open %s: %s\n",
                argv[1], strerror(errno));
        return 2;
    }

    rc_runner_init(&runner);
    result = rc_runner_process_stream(&runner, input);

    if (fclose(input) != 0) {
        perror("rc-runner: fclose");
        return 2;
    }

    rc_runner_print_summary(&runner);

    if (result == RC_RUN_IO_ERROR) {
        return 2;
    }

    return result == 0 ? 0 : 1;
}
