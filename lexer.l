%option noyywrap
%option nounistd
%option c++
%option yyclass="lexer"
%option prefix="cplus_"

%{
#include "lexer.h"
#include "shell.h"
#include "parser.hpp"
%}

digit      [0-9]
alpha      [a-zA-Z_]
alphanum   [a-zA-Z_0-9]
comment    "//"[^\n]*"\n"
string     \"[^\"]*\"

%%
comment                     /* ignore comments */
[ \t\n]                     /* ignore whitespaces */
"var"                       { std::cout << "[LEXER]: VAR\n";
                                return cplus::parser::make_VAR(); }
"is"                        { std::cout << "[LEXER]: IS\n";
                                return cplus::parser::make_IS(); }
true|false                  { std::cout << "[LEXER]: BOOLEAN_VAL\n";
                                return cplus::parser::make_BOOLEAN(); }
{alpha}{alphanum}*          { std::cout << "[LEXER]: IDENTIFIER\n";
                                return cplus::parser::make_ID(yytext); }
{digit}+\.{digit}+ 	        { std::cout << "[LEXER]: REAL_VAL\n";
                                return cplus::parser::make_REAL(); }
{digit}+		            { std::cout << "[LEXER]: INT_VAL\n";
                                return cplus::parser::make_INT(); }
";"                         { std::cout << "[LEXER]: SEMICOLON\n";
                                return cplus::parser::make_SEMICOLON(); }
.                           { std::cout << "[LEXER]: Unknown token: " << yytext << '\n'; }
<<EOF>>                     { return 0; }
%%
