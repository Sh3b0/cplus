#include <iostream>

#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "llvm.hpp"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

extern cplus::shell shell;
extern ast::np<ast::Program> program;

int main(int argc, char **argv) {
    
    std::cout << "\n\n" << YELLOW << "[LEXER]" << RESET << " and " << GREEN << "[PARSER]" << RESET << ":" << std::endl;
    
    if (shell.parse_args(argc, argv)) {
        std::cerr << RESET << RED << "Error parsing arguments\n";
        return 1;
    }

    if (shell.parse_program()) {
        std::cerr << RESET << RED << "Error parsing program\n";
        return 1;
    }

    std::cout << MAGENTA << "[AST]:" << std::endl;
    if(shell.debug) shell.print_ast();
    std::cout << RESET;

    std::cout << CYAN << "[LLVM]:" << RESET << std::endl;
    IRGenerator gen;
    program->accept(&gen);
    gen.generate();

    std::string cmd = "clang -x ir ir.ll -o " + shell.outfile;
    system(cmd.c_str());
    std::cout << "\033[0m" << "\nCompilation successful. Run ./" << shell.outfile << " to execute\n";
    
    return 0;
}