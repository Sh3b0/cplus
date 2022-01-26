#ifndef LEXER_H
#define LEXER_H

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer cplus_FlexLexer
#include <FlexLexer.h>
#endif

// Replaces `int yylex()` with `symbol_type get_next_token()` to be compatible with bison 3
#undef YY_DECL
#define YY_DECL cplus::parser::symbol_type cplus::lexer::get_next_token()

#include "parser.tab.hpp"
#include <fstream>

namespace cplus {
        
    class lexer : public yyFlexLexer {
    public:
        lexer() { }
        virtual ~lexer() {}
        virtual cplus::parser::symbol_type get_next_token();
    };

}

#endif // LEXER_H