%option nodefault
%option noyywrap
%option c++
%option yyclass="lexer"
%option prefix="cplus_"

%{
#include "lexer.h"
#include "parser.tab.hpp"

#define yyterminate() cplus::parser::make_END();
%}

digit         [0-9]
letter        [a-zA-Z]

%%
"var"                       {   std::cout << "[TOKEN]: VAR\n";
                                return cplus::parser::make_VAR(); }
"is"                        { std::cout << "[TOKEN]: IS\n";
                                return cplus::parser::make_IS(); }
{letter}({letter}|{digit})* { std::cout << "[TOKEN]: IDENTIFIER\n";
                                return cplus::parser::make_ID(); }
[0-9]+\.[0-9]+ 	            { std::cout << "[TOKEN]: REAL_V\n";
                                return cplus::parser::make_REAL(); }
[0-9]+		                { std::cout << "[TOKEN]: INT_V\n";
                                return cplus::parser::make_INT(); }
[ \t\n\r]                   /* skip whitespace */
.                           { std::cout << "[UNKNOWN_TOKEN]: " << yytext << '\n'; }
<<EOF>>                     { return yyterminate(); }
%%


/* 
int main()
{
    std::cout << "Hello\n";
    std::ifstream file("examples/ex1.cp");
    yyFlexLexer lexer(&file);

    cplus::parser p(lexer);
    std::cout << "Parser created\n";
    p.parse();
    return 0;
} */

int main() {
    cplus::lexer lexer();
    lexer.get_next_token();
	return 0;
}