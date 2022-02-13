#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "memory"

using namespace cplus;
using namespace std;

shell s;

extern ast::np<ast::Program> program;  // Points to the whole program node.

// TODO: data structures changed to map, modify this function accordingly.
void print_ast(ast::np<ast::Node> ast)
{
    std::cout << "Parsing complete. Printing AST:\n\n";
    std::cout << "PROGRAM ROOT\n";
    std::cout << "|\n";
    std::cout << "|-";

    std::cout << " Routines:\n";
    for (const std::shared_ptr<ast::Routine>& routine : program->routines)
    {
        std::cout << "|\t|\n";
        std::cout << "|\t|- ROUTINE\t" << routine->name;
        if (routine->params.empty())
        {
            std::cout << " NO PARAMETERS\n";
        }
        else
        {
            std::cout << "PARAMETERS";
            for (const auto& param : routine->params)
            {
                std::cout << param << " ";
                std::cout << "\n";
            }
        }
    }

    std::cout << "|\n";
    std::cout << "|-";

    std::cout << " Variables:\n";
    for (auto u: program->variables)
    {
        std::cout << "|\t|\n";
        // std::cout << "|\t|- VARIABLE\t" << u.first << " TYPE\t" << u.second << " VALUE\t" << var->value << "\n";
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