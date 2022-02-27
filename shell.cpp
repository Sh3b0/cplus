#include <sstream>
#include <fstream>
#include <chrono>

#include "shell.hpp"

extern ast::np<ast::Program> program;
cplus::shell shell;

namespace cplus {

shell::shell() : l(*this), p(l, *this) {}

int shell::parse_program() {
    return p.parse();
}

void shell::readFrom(std::istream *is) {
    l.switch_streams(is, nullptr);
}

void shell::prompt()
{
    if ((*this).interactive) {
        std::cout << "cplus> ";
    }
}

void shell::show_help() {
    std::cout << "usage: cplus [options] [file]\n\noptional arguments:\n";
    std::cout << "-h, --help\tshow this help message and exit.\n";
    std::cout << "-d, --debug\tshow debug messages.\n";
    std::cout << "-o, --outfile\texecutable file name.\n";
    std::cout << "file\t\tpath to the source code file (*.cp) to compile.\n";
}

int shell::parse_args(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            show_help();
            return 0;
        }
        else if (arg == "-d" || arg == "--debug") {
            debug = true;
        }
        else if (arg == "-o" || arg == "--outfile") {
            outfile = argv[++i];
            continue;
        }
        else {
            interactive = false;
            infile.open(arg);
            if (!infile.good())
            {
                std::cout << "Error: no such file: " << arg << '\n';
                return 1;
            }
            readFrom(&infile);
        }
    }
    if (!infile.is_open()) {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string now = ctime(&time);
        now.erase(now.size() - 1);
        std::cout << "C+ 0.1.0 (" << now << ")\n";
        prompt();
    }
    return 0;
}

int shell::print_ast() {
    std::cout << "[AST]: Routines: ";
    for(auto u:program->routines)
        std::cout << u->name << " ";

    std::cout << "\n[AST]: Global variables: ";
    for(auto u:program->variables)
        std::cout << u->name << " ";
    
    std::cout << std::endl;

    return 0;
}

} // namespace cplus
