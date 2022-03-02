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

#.*     ;       /* ignore comments */

[ \t\n] ;       /* ignore whitespaces */

"var" {
    if (driver.debug) std::cout << "[LEXER]: VAR\n";
    return cplus::parser::make_VAR();
}

"is" {
    if (driver.debug) std::cout << "[LEXER]: IS\n";
    return cplus::parser::make_IS();
}

"type" {
    if (driver.debug) std::cout << "[LEXER]: TYPE_KW\n";
    return cplus::parser::make_TYPE_KW();
}

"integer" {
    if (driver.debug) std::cout << "[LEXER]: INT_KW\n";
    return cplus::parser::make_INT_KW();
}

"real" {
    if (driver.debug) std::cout << "[LEXER]: REAL_KW\n";
    return cplus::parser::make_REAL_KW();
}

"boolean" {
    if (driver.debug) std::cout << "[LEXER]: BOOL_KW\n";
    return cplus::parser::make_BOOL_KW();
}

"array" {
    if (driver.debug) std::cout << "[LEXER]: ARRAY\n";
    return cplus::parser::make_ARRAY();
}

"record" {
    if (driver.debug) std::cout << "[LEXER]: RECORD\n";
    return cplus::parser::make_RECORD();
}

"routine" {
    if (driver.debug) std::cout << "[LEXER]: ROUTINE\n";
    return cplus::parser::make_ROUTINE();
}

"return" {
    if (driver.debug) std::cout << "[LEXER]: RETURN\n";
    return cplus::parser::make_RETURN();
}

"end" {
    if (driver.debug) std::cout << "[LEXER]: END\n";
    return cplus::parser::make_END();
}

"print" {
    if (driver.debug) std::cout << "[LEXER]: PRINT\n";
    return cplus::parser::make_PRINT();
}

"if" {
    if (driver.debug) std::cout << "[LEXER]: IF\n";
    return cplus::parser::make_IF();
}

"then" {
    if (driver.debug) std::cout << "[LEXER]: THEN\n";
    return cplus::parser::make_THEN();
}

"else" {
    if (driver.debug) std::cout << "[LEXER]: ELSE\n";
    return cplus::parser::make_ELSE();
}

"while" {
    if (driver.debug) std::cout << "[LEXER]: WHILE\n";
    return cplus::parser::make_WHILE();
}

"for" {
    if (driver.debug) std::cout << "[LEXER]: FOR\n";
    return cplus::parser::make_FOR();
}

"in" {
    if (driver.debug) std::cout << "[LEXER]: IN\n";
    return cplus::parser::make_IN();
}

"loop" {
    if (driver.debug) std::cout << "[LEXER]: LOOP\n";
    return cplus::parser::make_LOOP();
}

"reverse" {
    if (driver.debug) std::cout << "[LEXER]: REVERSE\n";
    return cplus::parser::make_REVERSE();
}

"and" {
    if (driver.debug) std::cout << "[LEXER]: AND\n";
    return cplus::parser::make_AND();
}

"or" {
    if (driver.debug) std::cout << "[LEXER]: OR\n";
    return cplus::parser::make_OR();
}

"xor" {
    if (driver.debug) std::cout << "[LEXER]: XOR\n";
    return cplus::parser::make_XOR();
}

"not" {
    if (driver.debug) std::cout << "[LEXER]: NOT\n";
    return cplus::parser::make_NOT();
}

"+" {
    if (driver.debug) std::cout << "[LEXER]: PLUS\n";
    return cplus::parser::make_PLUS();
}

"-" {
    if (driver.debug) std::cout << "[LEXER]: MINUS\n";
    return cplus::parser::make_MINUS();
}

"*" {
    if (driver.debug) std::cout << "[LEXER]: MUL\n";
    return cplus::parser::make_MUL();
}

"/" {
    if (driver.debug) std::cout << "[LEXER]: DIV\n";
    return cplus::parser::make_DIV();
}

"%" {
    if (driver.debug) std::cout << "[LEXER]: MOD\n";
    return cplus::parser::make_MOD();
}

";" {
    if (driver.debug) std::cout << "[LEXER]: SEMICOLON\n";
    return cplus::parser::make_SEMICOLON();
}

":" {
    if (driver.debug) std::cout << "[LEXER]: COLON\n";
    return cplus::parser::make_COLON();
}

"," {
    if (driver.debug) std::cout << "[LEXER]: COMMA\n";
    return cplus::parser::make_COMMA();
}

"(" {
    if (driver.debug) std::cout << "[LEXER]: B_L\n";
    return cplus::parser::make_B_L();
}

")" {
    if (driver.debug) std::cout << "[LEXER]: B_R\n";
    return cplus::parser::make_B_R();
}

"[" {
    if (driver.debug) std::cout << "[LEXER]: SB_L\n";
    return cplus::parser::make_SB_L();
}

"]" {
    if (driver.debug) std::cout << "[LEXER]: SB_R\n";
    return cplus::parser::make_SB_R();
}

"{" {
    if (driver.debug) std::cout << "[LEXER]: CB_L\n";
    return cplus::parser::make_CB_L();
}

"}" {
    if (driver.debug) std::cout << "[LEXER]: CB_R\n";
    return cplus::parser::make_CB_R();
}

"." {
    if (driver.debug) std::cout << "[LEXER]: DOT\n";
    return cplus::parser::make_DOT();
}

".." {
    if (driver.debug) std::cout << "[LEXER]: DDOT\n";
    return cplus::parser::make_DDOT();
}

":=" {
    if (driver.debug) std::cout << "[LEXER]: BECOMES\n";
    return cplus::parser::make_BECOMES();
}

"=" {
    if (driver.debug) std::cout << "[LEXER]: EQ\n";
    return cplus::parser::make_EQ();
}

"<" {
    if (driver.debug) std::cout << "[LEXER]: LT\n";
    return cplus::parser::make_LT();
}

"<=" {
    if (driver.debug) std::cout << "[LEXER]: LEQ\n";
    return cplus::parser::make_LEQ();
}

">" {
    if (driver.debug) std::cout << "[LEXER]: GT\n";
    return cplus::parser::make_GT();
}

">=" {
    if (driver.debug) std::cout << "[LEXER]: GEQ\n";
    return cplus::parser::make_GEQ();
}

"/=" {
    if (driver.debug) std::cout << "[LEXER]: NEQ\n";
    return cplus::parser::make_NEQ();
}

true {
    if (driver.debug) std::cout << "[LEXER]: BOOL_VAL\n";
    return cplus::parser::make_BOOL_VAL(true);
}

false {
    if (driver.debug) std::cout << "[LEXER]: BOOL_VAL\n";
    return cplus::parser::make_BOOL_VAL(false);
}

{alpha}{alphanum}* {
    if (driver.debug) std::cout << "[LEXER]: IDENTIFIER\n";
    return cplus::parser::make_ID(yytext);
}

-?{digit}+\.{digit}+ {
    if (driver.debug) std::cout << "[LEXER]: REAL_VAL\n";
    return cplus::parser::make_REAL_VAL(atof(yytext));
}

-?{digit}+ {
    if (driver.debug) std::cout << "[LEXER]: INT_VAL\n";
    return cplus::parser::make_INT_VAL(atoi(yytext));
}

. {
    std::cout << "[LEXER]: Unknown token: " << yytext << '\n';
}

<<EOF>> {
    if (driver.debug) std::cout << "[LEXER]: EOF \n";
    return 0;
}

%%
