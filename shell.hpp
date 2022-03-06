#ifndef SHELL_H
#define SHELL_H

#include <fstream>

#include "lexer.h"
#include "parser.hpp"

namespace cplus {
class shell {
public:
    shell();

    friend class parser;
    friend class lexer;

    bool debug = false;
    std::ifstream infile;
    std::string outfile = "a.out";

    int parse_program();
    int parse_args(int argc, char **argv);
    int print_ast();
    void readFrom(std::istream *is);
    void show_help();
    
private:
    lexer l;
    parser p;
};
} //namespace cplus

#endif // SHELL_H
