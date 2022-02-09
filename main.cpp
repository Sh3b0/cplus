#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "memory"

using namespace cplus;
using namespace std;

shell s;

extern ast::ast_node<ast::Program> program;  // Points to the whole program node.

void print_ast(ast::ast_node<ast::Node> ast)
{
    std::cout << "Parsing complete. Printing AST:\n\n";
    std::cout << "PROGRAM ROOT\n";
    std::cout << "|" << std::endl;
    std::cout << "|-";
    if (program->routines.empty())
    {
        std::cout << " Routines: * No routines were declared. *\n";
    }
    std::cout << "|\n";
    std::cout << "|-";

    std::cout << " Variables:\n";
    for (const std::shared_ptr<ast::Variable>& var : program->variables)
    {
        std::cout << "|\t|\n";
        std::cout << "|\t|- VARIABLE\t" << var->name << " TYPE\t" << var->dtype << " VALUE\t" << var->value << "\n";
    }
    std::cout << std::endl;
}

int main(int argc, char **argv)
{
    if (s.parse_args(argc, argv)) {
        cout << "Error parsing arguments\n";
        return 1;
    }
    if (s.parse_program()) {
        cout << "Error parsing program\n";
        return 1;
    }

    print_ast(program);

    return 0;
}