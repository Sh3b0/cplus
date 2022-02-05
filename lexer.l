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
"#"[^\n]*"\n"               ; /* ignore comments */
[ \t\n]                     ; /* ignore whitespaces */
"var"                       { if (m_driver.ldebug) std::cout << "[LEXER]: VAR\n";
                                return cplus::parser::make_VAR(); }
"is"                        { if (m_driver.ldebug) std::cout << "[LEXER]: IS\n";
                                return cplus::parser::make_IS(); }
"type"                      { if (m_driver.ldebug) std::cout << "[LEXER]: TYPE\n";
                                return cplus::parser::make_TYPE(); }
"integer"                   { if (m_driver.ldebug) std::cout << "[LEXER]: INT_KW\n";
                                return cplus::parser::make_INT_KW(); }
"real"                      { if (m_driver.ldebug) std::cout << "[LEXER]: REAL_KW\n";
                                return cplus::parser::make_REAL_KW(); }
"boolean"                   { if (m_driver.ldebug) std::cout << "[LEXER]: BOOLEAN_KW\n";
                                return cplus::parser::make_BOOLEAN_KW(); }
"array"                     { if (m_driver.ldebug) std::cout << "[LEXER]: ARRAY\n";
                                return cplus::parser::make_ARRAY(); }
"record"                    { if (m_driver.ldebug) std::cout << "[LEXER]: RECORD\n";
                                return cplus::parser::make_RECORD(); }
"routine"                   { if (m_driver.ldebug) std::cout << "[LEXER]: ROUTINE\n";
                                return cplus::parser::make_ROUTINE(); }
"end"                       { if (m_driver.ldebug) std::cout << "[LEXER]: END\n";
                                return cplus::parser::make_END(); }
";"                         { if (m_driver.ldebug) std::cout << "[LEXER]: SEMICOLON\n";
                                return cplus::parser::make_SEMICOLON(); }
":"                         { if (m_driver.ldebug) std::cout << "[LEXER]: COLON\n";
                                return cplus::parser::make_COLON(); }
","                         { if (m_driver.ldebug) std::cout << "[LEXER]: COMMA\n";
                                return cplus::parser::make_COMMA(); }
"("                         { if (m_driver.ldebug) std::cout << "[LEXER]: B_L\n";
                                return cplus::parser::make_B_L(); }
")"                         { if (m_driver.ldebug) std::cout << "[LEXER]: B_R\n";
                                return cplus::parser::make_B_R(); }
"["                         { if (m_driver.ldebug) std::cout << "[LEXER]: SB_L\n";
                                return cplus::parser::make_SB_L(); }
"]"                         { if (m_driver.ldebug) std::cout << "[LEXER]: SB_R\n";
                                return cplus::parser::make_SB_R(); }
"{"                         { if (m_driver.ldebug) std::cout << "[LEXER]: CB_L\n";
                                return cplus::parser::make_CB_L(); }
"}"                         { if (m_driver.ldebug) std::cout << "[LEXER]: CB_R\n";
                                return cplus::parser::make_CB_R(); }
true                        { if (m_driver.ldebug) std::cout << "[LEXER]: BOOLEAN_VAL\n";
                                return cplus::parser::make_BOOLEAN_VAL(true); }
false                       { if (m_driver.ldebug) std::cout << "[LEXER]: BOOLEAN_VAL\n";
                                return cplus::parser::make_BOOLEAN_VAL(false); }
{alpha}{alphanum}*          { if (m_driver.ldebug) std::cout << "[LEXER]: IDENTIFIER\n";
                                return cplus::parser::make_ID(yytext); }
{digit}+\.{digit}+ 	        { if (m_driver.ldebug) std::cout << "[LEXER]: REAL_VAL\n";
                                return cplus::parser::make_REAL_VAL(atof(yytext)); }
{digit}+		            { if (m_driver.ldebug) std::cout << "[LEXER]: INT_VAL\n";
                                return cplus::parser::make_INT_VAL(atoi(yytext)); }
.                           { std::cout << "[LEXER]: Unknown token: " << yytext << '\n'; }
<<EOF>>                     { return 0; }
%%
