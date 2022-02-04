
#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.hpp"
#include "shell.h"

using namespace cplus;
using namespace std;

int main(int argc, char **argv) {
    shell s;

    s.interactive = false;
    ifstream file("examples/ex1.cp");
    s.readFrom(&file);

    if (s.interactive) cout << "cplus> ";

    s.parse();
    return 0;
}