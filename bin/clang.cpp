#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void print_usage(const char *program)
{
    std::cerr
        << "Usage: " << program << " [options] source...\n"
        << "  -I DIR       Add an include directory\n"
        << "  -fPIE        Generate position-independent executable code\n"
        << "  -pie         Link a PIE executable\n"
        << "  -static      Request static linking\n"
        << "  --text FILE  Convert text bytes to hexadecimal\n"
        << "  --hex TEXT   Print text as hexadecimal bytes\n"
        << "  --bytes FILE Print file bytes as hexadecimal\n"
        << "  --help       Show this help\n";
}

static void print_hex(const unsigned char *data, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        std::printf("%02X", static_cast<unsigned int>(data[i]));

        if (i + 1 < length)
        {
            std::putchar(' ');
        }
    }

    std::putchar('\n');
}

static bool print_file_hex(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        std::cerr << "clang: cannot open '" << path << "'\n";
        return false;
    }

    unsigned char buffer[4096];

    while (file)
    {
        file.read(
            reinterpret_cast<char *>(buffer),
            sizeof(buffer)
        );

        const std::streamsize count = file.gcount();

        if (count > 0)
        {
            print_hex(
                buffer,
                static_cast<size_t>(count)
            );
        }
    }

    if (file.bad())
    {
        std::cerr << "clang: error reading '" << path << "'\n";
        return false;
    }

    return true;
}

static int run_clang(const std::vector<std::string> &arguments)
{
    std::vector<char *> child_arguments;
    child_arguments.reserve(arguments.size() + 1);

    for (const std::string &argument : arguments)
    {
        child_arguments.push_back(
            const_cast<char *>(argument.c_str())
        );
    }

    child_arguments.push_back(nullptr);

    pid_t child = fork();

    if (child < 0)
    {
        std::cerr
            << "clang: cannot start compiler: "
            << std::strerror(errno)
            << '\n';

        return 1;
    }

    if (child == 0)
    {
        execvp(child_arguments[0], child_arguments.data());

        std::cerr
            << "clang: cannot execute compiler: "
            << std::strerror(errno)
            << '\n';

        _exit(127);
    }

    int status = 0;

    while (waitpid(child, &status, 0) < 0)
    {
        if (errno == EINTR)
        {
            continue;
        }

        std::cerr
            << "clang: wait failed: "
            << std::strerror(errno)
            << '\n';

        return 1;
    }

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status))
    {
        std::cerr
            << "clang: compiler terminated by signal "
            << WTERMSIG(status)
            << '\n';

        return 128 + WTERMSIG(status);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    std::vector<std::string> compiler_arguments;
    compiler_arguments.push_back("clang");

    bool compile_requested = false;

    for (int i = 1; i < argc; ++i)
    {
        const std::string argument = argv[i];

        if (argument == "--help")
        {
            print_usage(argv[0]);
            return 0;
        }

        if (argument == "--hex")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "clang: --hex requires text\n";
                return 2;
            }

            const std::string text = argv[++i];

            print_hex(
                reinterpret_cast<const unsigned char *>(text.data()),
                text.size()
            );

            continue;
        }

        if (argument == "--bytes" || argument == "--text")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "clang: " << argument
                          << " requires a file\n";
                return 2;
            }

            if (!print_file_hex(argv[++i]))
            {
                return 1;
            }

            continue;
        }

        if (argument == "-I")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "clang: -I requires a directory\n";
                return 2;
            }

            compiler_arguments.push_back("-I");
            compiler_arguments.push_back(argv[++i]);
            compile_requested = true;
            continue;
        }

        if (argument.rfind("-I", 0) == 0 ||
            argument == "-fPIE" ||
            argument == "-fpie" ||
            argument == "-pie" ||
            argument == "-static" ||
            argument == "-c" ||
            argument == "-S" ||
            argument == "-E" ||
            argument.rfind("-o", 0) == 0 ||
            argument.rfind("-std=", 0) == 0 ||
            argument.rfind("-W", 0) == 0)
        {
            compiler_arguments.push_back(argument);
            compile_requested = true;
            continue;
        }

        if (!argument.empty() && argument[0] == '-')
        {
            compiler_arguments.push_back(argument);
            compile_requested = true;
            continue;
        }

        compiler_arguments.push_back(argument);
        compile_requested = true;
    }

    if (!compile_requested)
    {
        print_usage(argv[0]);
        return 2;
    }

    return run_clang(compiler_arguments);
}
