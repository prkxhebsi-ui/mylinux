#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_valid_name(const char *name)
{
    if (name == NULL || name[0] == '\0')
    {
        return 0;
    }

    if (!(isalpha((unsigned char)name[0]) || name[0] == '_'))
    {
        return 0;
    }

    for (size_t i = 1; name[i] != '\0'; ++i)
    {
        if (!(isalnum((unsigned char)name[i]) || name[i] == '_'))
        {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        for (char **entry = environ; *entry != NULL; ++entry)
        {
            puts(*entry);
        }

        return 0;
    }

    for (int i = 1; i < argc; ++i)
    {
        char *equals = strchr(argv[i], '=');

        if (equals == NULL)
        {
            const char *value = getenv(argv[i]);

            if (!is_valid_name(argv[i]))
            {
                fprintf(stderr, "export: invalid variable name: %s\n", argv[i]);
                return 2;
            }

            if (value == NULL)
            {
                fprintf(stderr, "export: %s is not set\n", argv[i]);
                continue;
            }

            printf("%s=%s\n", argv[i], value);
            continue;
        }

        size_t name_length = (size_t)(equals - argv[i]);

        char *name = malloc(name_length + 1);

        if (name == NULL)
        {
            perror("export: malloc");
            return 1;
        }

        memcpy(name, argv[i], name_length);
        name[name_length] = '\0';

        if (!is_valid_name(name))
        {
            fprintf(stderr, "export: invalid assignment: %s\n", argv[i]);
            free(name);
            return 2;
        }

        printf("%s=%s\n", name, equals + 1);
        free(name);
    }

    return 0;
}
