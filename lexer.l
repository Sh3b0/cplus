%option noyywrap
%option nounistd
%option c++
%option yyclass="lexer"
%option prefix="cplus_"

%{
#include "lexer.h"
#include "shell.hpp"
#include "parser.hpp"
%}

digit      [0-9]
alpha      [a-zA-Z_]
alphanum   [a-zA-Z_0-9]

%%

"#"[^\n]*"\n" ; /* ignore comments */

[ \t\n] ;       /* ignore whitespaces */

"var" {
    if (m_driver.ldebug) std::cout << "[LEXER]: VAR\n";
    return cplus::parser::make_VAR();
}

"is" {
    if (m_driver.ldebug) std::cout << "[LEXER]: IS\n";
    return cplus::parser::make_IS();
}

"type" {
    if (m_driver.ldebug) std::cout << "[LEXER]: TYPE\n";
    return cplus::parser::make_TYPE();
}

"integer" {
    if (m_driver.ldebug) std::cout << "[LEXER]: INT_KW\n";
    return cplus::parser::make_INT_KW();
}

"real" {
    if (m_driver.ldebug) std::cout << "[LEXER]: REAL_KW\n";
    return cplus::parser::make_REAL_KW();
}

"boolean" {
    if (m_driver.ldebug) std::cout << "[LEXER]: BOOLEAN_KW\n";
    return cplus::parser::make_BOOLEAN_KW();
}

"array" {
    if (m_driver.ldebug) std::cout << "[LEXER]: ARRAY\n";
    return cplus::parser::make_ARRAY();
}

"record" {
    if (m_driver.ldebug) std::cout << "[LEXER]: RECORD\n";
    return cplus::parser::make_RECORD();
}

"routine" {
    if (m_driver.ldebug) std::cout << "[LEXER]: ROUTINE\n";
    return cplus::parser::make_ROUTINE();
}

"return" {
    if (m_driver.ldebug) std::cout << "[LEXER]: RETURN\n";
    return cplus::parser::make_RETURN();
}

"end" {
    if (m_driver.ldebug) std::cout << "[LEXER]: END\n";
    return cplus::parser::make_END();
}

"print" {
    if (m_driver.ldebug) std::cout << "[LEXER]: PRINT\n";
    return cplus::parser::make_PRINT();
}

"if" {
    if (m_driver.ldebug) std::cout << "[LEXER]: IF\n";
    return cplus::parser::make_IF();
}

"then" {
    if (m_driver.ldebug) std::cout << "[LEXER]: THEN\n";
    return cplus::parser::make_THEN();
}

"else" {
    if (m_driver.ldebug) std::cout << "[LEXER]: ELSE\n";
    return cplus::parser::make_ELSE();
}

"while" {
    if (m_driver.ldebug) std::cout << "[LEXER]: WHILE\n";
    return cplus::parser::make_WHILE();
}

"for" {
    if (m_driver.ldebug) std::cout << "[LEXER]: FOR\n";
    return cplus::parser::make_FOR();
}

"in" {
    if (m_driver.ldebug) std::cout << "[LEXER]: IN\n";
    return cplus::parser::make_IN();
}

"loop" {
    if (m_driver.ldebug) std::cout << "[LEXER]: LOOP\n";
    return cplus::parser::make_LOOP();
}

"reverse" {
    if (m_driver.ldebug) std::cout << "[LEXER]: REVERSE\n";
    return cplus::parser::make_REVERSE();
}

"and" {
    if (m_driver.ldebug) std::cout << "[LEXER]: AND\n";
    return cplus::parser::make_AND();
}

"or" {
    if (m_driver.ldebug) std::cout << "[LEXER]: OR\n";
    return cplus::parser::make_OR();
}

"xor" {
    if (m_driver.ldebug) std::cout << "[LEXER]: XOR\n";
    return cplus::parser::make_XOR();
}

"not" {
    if (m_driver.ldebug) std::cout << "[LEXER]: NOT\n";
    return cplus::parser::make_NOT();
}

"+" {
    if (m_driver.ldebug) std::cout << "[LEXER]: PLUS\n";
    return cplus::parser::make_PLUS();
}

"-" {
    if (m_driver.ldebug) std::cout << "[LEXER]: MINUS\n";
    return cplus::parser::make_MINUS();
}

"*" {
    if (m_driver.ldebug) std::cout << "[LEXER]: MUL\n";
    return cplus::parser::make_MUL();
}

"/" {
    if (m_driver.ldebug) std::cout << "[LEXER]: DIV\n";
    return cplus::parser::make_DIV();
}

"%" {
    if (m_driver.ldebug) std::cout << "[LEXER]: MOD\n";
    return cplus::parser::make_MOD();
}

";" {
    if (m_driver.ldebug) std::cout << "[LEXER]: SEMICOLON\n";
    return cplus::parser::make_SEMICOLON();
}

":" {
    if (m_driver.ldebug) std::cout << "[LEXER]: COLON\n";
    return cplus::parser::make_COLON();
}

"," {
    if (m_driver.ldebug) std::cout << "[LEXER]: COMMA\n";
    return cplus::parser::make_COMMA();
}

"(" {
    if (m_driver.ldebug) std::cout << "[LEXER]: B_L\n";
    return cplus::parser::make_B_L();
}

")" {
    if (m_driver.ldebug) std::cout << "[LEXER]: B_R\n";
    return cplus::parser::make_B_R();
}

"[" {
    if (m_driver.ldebug) std::cout << "[LEXER]: SB_L\n";
    return cplus::parser::make_SB_L();
}

"]" {
    if (m_driver.ldebug) std::cout << "[LEXER]: SB_R\n";
    return cplus::parser::make_SB_R();
}

"{" {
    if (m_driver.ldebug) std::cout << "[LEXER]: CB_L\n";
    return cplus::parser::make_CB_L();
}

"}" {
    if (m_driver.ldebug) std::cout << "[LEXER]: CB_R\n";
    return cplus::parser::make_CB_R();
}

"." {
    if (m_driver.ldebug) std::cout << "[LEXER]: DOT\n";
    return cplus::parser::make_DOT();
}

".." {
    if (m_driver.ldebug) std::cout << "[LEXER]: DDOT\n";
    return cplus::parser::make_DDOT();
}

":=" {
    if (m_driver.ldebug) std::cout << "[LEXER]: BECOMES\n";
    return cplus::parser::make_BECOMES();
}

"=" {
    if (m_driver.ldebug) std::cout << "[LEXER]: EQ\n";
    return cplus::parser::make_EQ();
}

"<" {
    if (m_driver.ldebug) std::cout << "[LEXER]: LT\n";
    return cplus::parser::make_LT();
}

"<=" {
    if (m_driver.ldebug) std::cout << "[LEXER]: LEQ\n";
    return cplus::parser::make_LEQ();
}

">" {
    if (m_driver.ldebug) std::cout << "[LEXER]: GT\n";
    return cplus::parser::make_GT();
}

">=" {
    if (m_driver.ldebug) std::cout << "[LEXER]: GEQ\n";
    return cplus::parser::make_GEQ();
}

"/=" {
    if (m_driver.ldebug) std::cout << "[LEXER]: NEQ\n";
    return cplus::parser::make_NEQ();
}

true {
    if (m_driver.ldebug) std::cout << "[LEXER]: BOOL_VAL\n";
    return cplus::parser::make_BOOL_VAL(true);
}

false {
    if (m_driver.ldebug) std::cout << "[LEXER]: BOOL_VAL\n";
    return cplus::parser::make_BOOL_VAL(false);
}

{alpha}{alphanum}* {
    if (m_driver.ldebug) std::cout << "[LEXER]: IDENTIFIER\n";
    return cplus::parser::make_ID(yytext);
}

{digit}+\.{digit}+ {
    if (m_driver.ldebug) std::cout << "[LEXER]: REAL_VAL\n";
    return cplus::parser::make_REAL_VAL(atof(yytext));
}

{digit}+ {
    if (m_driver.ldebug) std::cout << "[LEXER]: INT_VAL\n";
    return cplus::parser::make_INT_VAL(atoi(yytext));
}

. {
    std::cout << "[LEXER]: Unknown token: " << yytext << '\n';
}

<<EOF>> {
    return 0;
}

%%
