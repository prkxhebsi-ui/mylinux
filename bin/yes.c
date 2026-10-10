#include <stdio.h>

int main(int argc, char *argv[])
{
const char *text = "y";

if (argc > 1)
{
    text = argv[1];
}

while (1)
{
    puts(text);
}

return 0;

}
