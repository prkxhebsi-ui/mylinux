/*
 * MyLinux .rc Parser
 *
 * Parses a small subset of Android init configuration syntax.
 * This is a syntax-scanning prototype, not Android init itself.
 *
 * Supported line forms:
 *   on early-init
 *   on init
 *   on boot
 *   service name /path/to/program [arguments...]
 *   import /path/to/file.rc
 *   # comments and blank lines
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

#define RC_MAX_LINE       1024
#define RC_MAX_NAME        128
#define RC_MAX_PATH        512
#define RC_MAX_ARGUMENTS    32

typedef enum {
    RC_LINE_EMPTY = 0,
    RC_LINE_COMMENT,
    RC_LINE_TRIGGER,
    RC_LINE_SERVICE,
    RC_LINE_IMPORT,
    RC_LINE_COMMAND,
    RC_LINE_UNKNOWN,
    RC_LINE_ERROR
} rc_line_type_t;

typedef struct {
    rc_line_type_t type;
    unsigned int line_number;
    char name[RC_MAX_NAME];
    char path[RC_MAX_PATH];
    char arguments[RC_MAX_ARGUMENTS][RC_MAX_NAME];
    size_t argument_count;
} rc_line_t;

static void rc_trim(char *text)
{
    size_t length;
    size_t start = 0;

    if (text == NULL) {
        return;
    }

    length = strlen(text);

    while (start < length && isspace((unsigned char)text[start])) {
        start++;
    }

    if (start > 0) {
        memmove(text, text + start, length - start + 1);
        length -= start;
    }

    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
}

static int rc_copy_token(
    char *destination,
    size_t destination_size,
    const char *source)
{
    size_t length;

    if (destination == NULL ||
        destination_size == 0 ||
        source == NULL) {
        return -1;
    }

    length = strlen(source);

    if (length >= destination_size) {
        return -1;
    }

    memcpy(destination, source, length + 1);
    return 0;
}

static char *rc_next_token(char **cursor)
{
    char *start;

    if (cursor == NULL || *cursor == NULL) {
        return NULL;
    }

    while (**cursor != '\0' &&
           isspace((unsigned char)**cursor)) {
        (*cursor)++;
    }

    if (**cursor == '\0') {
        return NULL;
    }

    start = *cursor;

    while (**cursor != '\0' &&
           !isspace((unsigned char)**cursor)) {
        (*cursor)++;
    }

    if (**cursor != '\0') {
        *(*cursor)++ = '\0';
    }

    return start;
}

static void rc_line_reset(rc_line_t *result, unsigned int line_number)
{
    if (result == NULL) {
        return;
    }

    memset(result, 0, sizeof(*result));
    result->line_number = line_number;
    result->type = RC_LINE_EMPTY;
}

static int rc_parse_line(
    const char *input,
    unsigned int line_number,
    rc_line_t *result)
{
    char buffer[RC_MAX_LINE];
    char *cursor;
    char *first;

    if (input == NULL || result == NULL) {
        return -1;
    }

    if (strlen(input) >= sizeof(buffer)) {
        rc_line_reset(result, line_number);
        result->type = RC_LINE_ERROR;
        return -1;
    }

    rc_copy_token(buffer, sizeof(buffer), input);
    rc_trim(buffer);
    rc_line_reset(result, line_number);

    if (buffer[0] == '\0') {
        result->type = RC_LINE_EMPTY;
        return 0;
    }

    if (buffer[0] == '#') {
        result->type = RC_LINE_COMMENT;
        return 0;
    }

    cursor = buffer;
    first = rc_next_token(&cursor);

    if (first == NULL) {
        result->type = RC_LINE_EMPTY;
        return 0;
    }

    if (strcmp(first, "on") == 0) {
        char *trigger = rc_next_token(&cursor);

        if (trigger == NULL ||
            rc_copy_token(result->name,
                          sizeof(result->name),
                          trigger) != 0) {
            result->type = RC_LINE_ERROR;
            return -1;
        }

        result->type = RC_LINE_TRIGGER;
        return 0;
    }

    if (strcmp(first, "import") == 0) {
        char *path = rc_next_token(&cursor);

        if (path == NULL ||
            rc_copy_token(result->path,
                          sizeof(result->path),
                          path) != 0) {
            result->type = RC_LINE_ERROR;
            return -1;
        }

        result->type = RC_LINE_IMPORT;
        return 0;
    }

    if (strcmp(first, "service") == 0) {
        char *name = rc_next_token(&cursor);
        char *path = rc_next_token(&cursor);

        if (name == NULL || path == NULL ||
            rc_copy_token(result->name,
                          sizeof(result->name),
                          name) != 0 ||
            rc_copy_token(result->path,
                          sizeof(result->path),
                          path) != 0) {
            result->type = RC_LINE_ERROR;
            return -1;
        }

        result->type = RC_LINE_SERVICE;

        while (result->argument_count < RC_MAX_ARGUMENTS) {
            char *argument = rc_next_token(&cursor);

            if (argument == NULL) {
                break;
            }

            if (rc_copy_token(
                    result->arguments[result->argument_count],
                    sizeof(result->arguments[0]),
                    argument) != 0) {
                result->type = RC_LINE_ERROR;
                return -1;
            }

            result->argument_count++;
        }

        if (rc_next_token(&cursor) != NULL) {
            result->type = RC_LINE_ERROR;
            return -1;
        }

        return 0;
    }

    /*
     * A command is recognized only after an action trigger.
     * Full Android init command validation is not implemented here.
     */
    result->type = RC_LINE_COMMAND;
    rc_copy_token(result->name, sizeof(result->name), first);

    return 0;
}

static const char *rc_type_name(rc_line_type_t type)
{
    switch (type) {
        case RC_LINE_EMPTY:   return "empty";
        case RC_LINE_COMMENT: return "comment";
        case RC_LINE_TRIGGER: return "trigger";
        case RC_LINE_SERVICE: return "service";
        case RC_LINE_IMPORT:  return "import";
        case RC_LINE_COMMAND: return "command";
        case RC_LINE_UNKNOWN: return "unknown";
        case RC_LINE_ERROR:   return "error";
        default:              return "invalid";
    }
}

int mylinux_rc_parse_stream(FILE *stream)
{
    char line[RC_MAX_LINE];
    unsigned int line_number = 0;
    int errors = 0;

    if (stream == NULL) {
        return -1;
    }

    while (fgets(line, sizeof(line), stream) != NULL) {
        rc_line_t parsed;

        line_number++;

        if (strchr(line, '\n') == NULL && !feof(stream)) {
            int character;

            while ((character = fgetc(stream)) != '\n' &&
                   character != EOF) {
                /* Discard an overlong line. */
            }

            fprintf(stderr,
                    "rc_parser: line %u is too long\n",
                    line_number);
            errors++;
            continue;
        }

        if (rc_parse_line(line, line_number, &parsed) != 0) {
            fprintf(stderr,
                    "rc_parser: syntax error at line %u\n",
                    line_number);
            errors++;
            continue;
        }

        printf("%u: %s",
               parsed.line_number,
               rc_type_name(parsed.type));

        if (parsed.name[0] != '\0') {
            printf(" name=%s", parsed.name);
        }

        if (parsed.path[0] != '\0') {
            printf(" path=%s", parsed.path);
        }

        printf("\n");
    }

    if (ferror(stream)) {
        perror("rc_parser: read");
        return -1;
    }

    return errors;
}

#ifndef MYLINUX_RC_PARSER_NO_MAIN
int main(int argc, char **argv)
{
    FILE *input;
    int result;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.rc\n", argv[0]);
        return 2;
    }

    input = fopen(argv[1], "r");

    if (input == NULL) {
        perror(argv[1]);
        return 2;
    }

    result = mylinux_rc_parse_stream(input);
    fclose(input);

    return result == 0 ? 0 : 1;
}
#endif
