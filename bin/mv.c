#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: mv SOURCE DESTINATION\n");
        return 2;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    if (rename(source, destination) != 0)
    {
        fprintf(
            stderr,
            "mv: cannot move '%s' to '%s': %s\n",
            source,
            destination,
            strerror(errno)
        );

        return 1;
    }

    return 0;
}
