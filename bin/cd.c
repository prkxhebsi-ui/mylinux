#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    const char *target;

    if (argc > 2)
    {
        fprintf(stderr, "cd: too many arguments\n");
        return 2;
    }

    if (argc == 2)
    {
        target = argv[1];
    }
    else
    {
        target = getenv("HOME");

        if (target == NULL || target[0] == '\0')
        {
            fprintf(stderr, "cd: HOME is not set\n");
            return 1;
        }
    }

    if (strcmp(target, "-") == 0)
    {
        fprintf(
            stderr,
            "cd: cd - requires shell state; implement it as a shell builtin\n"
        );

        return 2;
    }

    if (chdir(target) != 0)
    {
        fprintf(
            stderr,
            "cd: %s: %s\n",
            target,
            strerror(errno)
        );

        return 1;
    }

    return 0;
}
