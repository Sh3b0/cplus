#include "lexer.h"
#include "parser.hpp"
#include "shell.hpp"

using namespace cplus;
using namespace std;

shell s;

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
    return 0;
}