#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

typedef int (*mylinux_script_executor_t)(FILE *script, const char *path);

/*
 * The shell provides this function so sourced commands can
 * modify the current shell's variables and working directory.
 */
extern int mylinux_execute_script(FILE *script, const char *path);

int mylinux_source_file(const char *path)
{
    if (path == NULL || path[0] == '\0')
    {
        fprintf(stderr, "source: missing file name\n");
        return 2;
    }

    FILE *script = fopen(path, "r");

    if (script == NULL)
    {
        perror("source: cannot open file");
        return 1;
    }

    int result = mylinux_execute_script(script, path);

    if (fclose(script) != 0)
    {
        perror("source: cannot close file");

        if (result == 0)
        {
            result = 1;
        }
    }

    return result;
}
