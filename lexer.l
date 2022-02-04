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

%%
"#"[^\n]*"\n"               /* ignore comments */
[ \t\n]                     /* ignore whitespaces */
"var"                       { std::cout << "[LEXER]: VAR\n";
                                return cplus::parser::make_VAR(); }
"is"                        { std::cout << "[LEXER]: IS\n";
                                return cplus::parser::make_IS(); }
"type"                      { std::cout << "[LEXER]: TYPE\n";
                                return cplus::parser::make_TYPE(); }
"integer"                   { std::cout << "[LEXER]: INT_KW\n";
                                return cplus::parser::make_INT_KW(); }
"real"                      { std::cout << "[LEXER]: REAL_KW\n";
                                return cplus::parser::make_REAL_KW(); }
"boolean"                   { std::cout << "[LEXER]: BOOLEAN_KW\n";
                                return cplus::parser::make_BOOLEAN_KW(); }
"array"                     { std::cout << "[LEXER]: ARRAY\n";
                                return cplus::parser::make_ARRAY(); }
"record"                    { std::cout << "[LEXER]: RECORD\n";
                                return cplus::parser::make_RECORD(); }
"routine"                   { std::cout << "[LEXER]: ROUTINE\n";
                                return cplus::parser::make_ROUTINE(); }
"end"                       { std::cout << "[LEXER]: END\n";
                                return cplus::parser::make_END(); }
";"                         { std::cout << "[LEXER]: SEMICOLON\n";
                                return cplus::parser::make_SEMICOLON(); }
":"                         { std::cout << "[LEXER]: COLON\n";
                                return cplus::parser::make_COLON(); }
","                         { std::cout << "[LEXER]: COMMA\n";
                                return cplus::parser::make_COMMA(); }
"("                         { std::cout << "[LEXER]: B_L\n";
                                return cplus::parser::make_B_L(); }
")"                         { std::cout << "[LEXER]: B_R\n";
                                return cplus::parser::make_B_R(); }
"["                         { std::cout << "[LEXER]: SB_L\n";
                                return cplus::parser::make_SB_L(); }
"]"                         { std::cout << "[LEXER]: SB_R\n";
                                return cplus::parser::make_SB_R(); }
"{"                         { std::cout << "[LEXER]: CB_L\n";
                                return cplus::parser::make_CB_L(); }
"}"                         { std::cout << "[LEXER]: CB_R\n";
                                return cplus::parser::make_CB_R(); }
true                        { std::cout << "[LEXER]: BOOLEAN_VAL\n";
                                return cplus::parser::make_BOOLEAN_VAL(true); }
false                       { std::cout << "[LEXER]: BOOLEAN_VAL\n";
                                return cplus::parser::make_BOOLEAN_VAL(false); }
{alpha}{alphanum}*          { std::cout << "[LEXER]: IDENTIFIER\n";
                                return cplus::parser::make_ID(yytext); }
{digit}+\.{digit}+ 	        { std::cout << "[LEXER]: REAL_VAL\n";
                                return cplus::parser::make_REAL_VAL(atof(yytext)); }
{digit}+		            { std::cout << "[LEXER]: INT_VAL\n";
                                return cplus::parser::make_INT_VAL(atoi(yytext)); }
.                           { std::cout << "[LEXER]: Unknown token: " << yytext << '\n'; }
<<EOF>>                     { return 0; }
%%
