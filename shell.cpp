#include <sstream>
#include <fstream>
#include <chrono>
#include "shell.hpp"

using namespace cplus;

shell::shell() : l(*this), p(l, *this) {}

int shell::parse_program()
{
    return p.parse();
}

void shell::readFrom(std::istream *is)
{
    l.switch_streams(is, NULL);
}

void shell::prompt()
{
    if ((*this).interactive)
    {
        cout << "cplus> ";
    }
}

void shell::show_help()
{
    cout << "usage: cplus [options] [file]\n\noptional arguments:\n";
    cout << "-h, --help\tshow this help message and exit.\n";
    cout << "-l, --lex-debug\tshow debug messages from lexer.\n";
    cout << "-p, --parse-debug\tshow debug messages from parser.\n";
    cout << "file\t\tpath to the source code file (*.cp) to execute.\n";
}

int shell::parse_args(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "--help" || arg == "-h")
        {
            show_help();
            return 0;
        }
        else if (arg == "--lex-debug" || arg == "-l")
        {
            ldebug = true;
        }
        else if (arg == "--parse-debug" || arg == "-p")
        {
            pdebug = true;
        }
        else if (arg == "-lp" || arg == "-pl")
        {
            ldebug = true;
            pdebug = true;
        }
        else
        {
            interactive = false;
            file.open(arg);
            if (!file.good())
            {
                cout << "Error: no such file: " << arg << '\n';
                return 1;
            }
            readFrom(&file);
        }
    }
    if (!file.is_open()) {
        auto time = chrono::system_clock::to_time_t(chrono::system_clock::now());
        string now = ctime(&time);
        now.erase(now.size() - 1);
        cout << "C+ 0.1.0 (" << now << ")\n";
        prompt();
    }
    return 0;
}
