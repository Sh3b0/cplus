#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"
#include "memory"

using namespace cplus;
using namespace std;

shell s;

int main(int argc, char **argv)
{
    if (s.parse_args(argc, argv)) {
        cout << "Error while parsing arguments\n";
        return 1;
    }
    if (s.parse_program()) {
        cout << "Error while parsing program\n";
        return 1;
    }

    s.print_ast();

    return 0;
}