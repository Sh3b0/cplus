#ifndef LEXER_H
#define LEXER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer cplus_FlexLexer
#include <FlexLexer.h>
#endif

// Replaces `int yylex()` with `symbol_type get_next_token()` to be compatible with bison 3
#undef YY_DECL
#define YY_DECL cplus::Parser::symbol_type cplus::Lexer::get_next_token()

#include "parser.hpp"
#include <fstream>

namespace cplus {
    class Shell; 
    
    class Lexer : public yyFlexLexer {
    public:
        Lexer(Shell& shell) : driver(shell) { }
        virtual ~Lexer() {}
        virtual cplus::Parser::symbol_type get_next_token();
        
    private:
        Shell &driver;
    };

}

#endif // LEXER_H