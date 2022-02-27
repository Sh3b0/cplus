#include <sstream>
#include <fstream>
#include <chrono>

#include "shell.hpp"

extern ast::np<ast::Program> program;
cplus::shell shell;

cplus::shell::shell() : l(*this), p(l, *this) {}

int cplus::shell::parse_program() {
    return p.parse();
}

void cplus::shell::readFrom(std::istream *is) {
    l.switch_streams(is, nullptr);
}

void cplus::shell::prompt()
{
    if ((*this).interactive) {
        std::cout << "cplus> ";
    }
}

void cplus::shell::show_help() {
    std::cout << "usage: cplus [options] [file]\n\noptional arguments:\n";
    std::cout << "-h, --help\tshow this help message and exit.\n";
    std::cout << "-v, --verbose\tshow debug messages.\n";
    std::cout << "file\t\tpath to the source code file (*.cp) to compile.\n";
}

int cplus::shell::parse_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            show_help();
            return 0;
        }
        else if (arg == "-d" || arg == "--debug") {
            debug = true;
        }
        else {
            interactive = false;
            file.open(arg);
            if (!file.good())
            {
                std::cout << "Error: no such file: " << arg << '\n';
                return 1;
            }
            readFrom(&file);
        }
    }
    if (!file.is_open()) {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string now = ctime(&time);
        now.erase(now.size() - 1);
        std::cout << "C+ 0.1.0 (" << now << ")\n";
        prompt();
    }
    return 0;
}

// int cplus::shell::print_ast() {
//     cout << "\nRoutines\n==========\n";
//     for(auto u:program->routines)
//         cout << "\t" << *u.second << '\n';

//     cout << "\nVariables\n==========\n";
//     for(auto u:program->variables)
//         cout << "\t" << *u.second << '\n';
    
//     cout << "\nType aliases\n=============\n";
//     for(auto u:program->types)
//         cout << "\t" << u.first << " -> " << *u.second << '\n';
    
//     return 0;
// }