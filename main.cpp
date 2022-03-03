#include <iostream>

#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "llvm.hpp"

extern cplus::shell shell;
extern ast::np<ast::Program> program;

int main(int argc, char **argv) {
    if (shell.parse_args(argc, argv)) {
        std::cout << "Error parsing arguments\n";
        return 1;
    }
    if (shell.parse_program()) {
        std::cout << "Error parsing program\n";
        return 1;
    }

    if(shell.debug) shell.print_ast();

    IRGenerator gen;
    program->accept(&gen);
    gen.generate();

    std::string cmd = "clang -x ir ir.ll -o " + shell.outfile;
    system(cmd.c_str());
    std::cout << "\033[0m" << "\nCompilation successful. Run ./" << shell.outfile << " to execute\n";
    
    return 0;
}