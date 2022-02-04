#ifndef SHELL_H
#define SHELL_H

#include "lexer.h"
#include "parser.hpp"

namespace cplus {
    class shell {
    public:
        shell();
        int parse();
        void readFrom(std::istream *is);
        void prompt();
        friend class parser;
        friend class lexer;
        int interactive = true;
    private:
        lexer l;
        parser p;
    };
}

#endif // SHELL_H
