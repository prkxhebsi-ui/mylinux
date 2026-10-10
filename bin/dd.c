#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct dd_options
{
    const char *input_path;
    const char *output_path;
    size_t block_size;
    uint64_t count;
    uint64_t skip;
    int count_set;
};

static void print_usage(const char *program)
{
    fprintf(
        stderr,
        "Usage: %s if=FILE of=FILE [bs=SIZE] [count=N] [skip=N]\n",
        program
    );
}

static int parse_u64(const char *text, uint64_t *value)
{
    char *end = NULL;

    if (text == NULL || text[0] == '\0' || text[0] == '-')
    {
        return -1;
    }

    errno = 0;

    unsigned long long parsed = strtoull(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0')
    {
        return -1;
    }

    *value = (uint64_t)parsed;
    return 0;
}

static int parse_size(const char *text, size_t *value)
{
    uint64_t parsed;

    if (parse_u64(text, &parsed) != 0 ||
        parsed == 0 ||
        parsed > SIZE_MAX)
    {
        return -1;
    }

    *value = (size_t)parsed;
    return 0;
}

static int parse_options(
    int argc,
    char *argv[],
    struct dd_options *options
)
{
    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "if=", 3) == 0)
        {
            options->input_path = argv[i] + 3;
        }
        else if (strncmp(argv[i], "of=", 3) == 0)
        {
            options->output_path = argv[i] + 3;
        }
        else if (strncmp(argv[i], "bs=", 3) == 0)
        {
            if (parse_size(argv[i] + 3, &options->block_size) != 0)
            {
                fprintf(stderr, "dd: invalid block size\n");
                return -1;
            }
        }
        else if (strncmp(argv[i], "count=", 6) == 0)
        {
            if (parse_u64(argv[i] + 6, &options->count) != 0)
            {
                fprintf(stderr, "dd: invalid count\n");
                return -1;
            }

            options->count_set = 1;
        }
        else if (strncmp(argv[i], "skip=", 5) == 0)
        {
            if (parse_u64(argv[i] + 5, &options->skip) != 0)
            {
                fprintf(stderr, "dd: invalid skip count\n");
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "dd: unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    if (options->input_path == NULL ||
        options->input_path[0] == '\0' ||
        options->output_path == NULL ||
        options->output_path[0] == '\0')
    {
        fprintf(stderr, "dd: both if= and of= are required\n");
        return -1;
    }

    return 0;
}

static int write_all(int fd, const unsigned char *buffer, size_t length)
{
    size_t written = 0;

    while (written < length)
    {
        ssize_t result = write(
            fd,
            buffer + written,
            length - written
        );

        if (result < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            return -1;
        }

        if (result == 0)
        {
            errno = EIO;
            return -1;
        }

        written += (size_t)result;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct dd_options options =
    {
        .input_path = NULL,
        .output_path = NULL,
        .block_size = 512,
        .count = 0,
        .skip = 0,
        .count_set = 0
    };

    if (parse_options(argc, argv, &options) != 0)
    {
        print_usage(argv[0]);
        return 2;
    }

    if (strcmp(options.input_path, options.output_path) == 0)
    {
        fprintf(stderr, "dd: input and output paths must differ\n");
        return 2;
    }

    int input_fd = open(options.input_path, O_RDONLY);

    if (input_fd < 0)
    {
        perror("dd: opening input");
        return 1;
    }

    for (uint64_t i = 0; i < options.skip; ++i)
    {
        unsigned char discard[4096];
        size_t remaining = options.block_size;

        while (remaining > 0)
        {
            size_t chunk = remaining < sizeof(discard)
                ? remaining
                : sizeof(discard);

            ssize_t result = read(input_fd, discard, chunk);

            if (result < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                perror("dd: skipping input");
                close(input_fd);
                return 1;
            }

            if (result == 0)
            {
                goto input_finished;
            }

            remaining -= (size_t)result;
        }
    }

    int output_fd = open(
        options.output_path,
        O_WRONLY | O_CREAT | O_TRUNC,
        0666
    );

    if (output_fd < 0)
    {
        perror("dd: opening output");
        close(input_fd);
        return 1;
    }

    unsigned char *buffer = malloc(options.block_size);

    if (buffer == NULL)
    {
        perror("dd: allocating buffer");
        close(input_fd);
        close(output_fd);
        return 1;
    }

    uint64_t blocks = 0;

    while (!options.count_set || blocks < options.count)
    {
        ssize_t bytes_read = read(
            input_fd,
            buffer,
            options.block_size
        );

        if (bytes_read < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }

            perror("dd: reading input");
            free(buffer);
            close(input_fd);
            close(output_fd);
            return 1;
        }

        if (bytes_read == 0)
        {
            break;
        }

        if (write_all(output_fd, buffer, (size_t)bytes_read) != 0)
        {
            perror("dd: writing output");
            free(buffer);
            close(input_fd);
            close(output_fd);
            return 1;
        }

        ++blocks;
    }

    free(buffer);
    close(input_fd);

    if (close(output_fd) != 0)
    {
        perror("dd: closing output");
        return 1;
    }

    fprintf(stderr, "dd: copied %" PRIu64 " blocks\n", blocks);
    return 0;

input_finished:
    close(input_fd);
    return 0;
}
