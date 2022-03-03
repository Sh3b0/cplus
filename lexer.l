%option noyywrap
%option nounistd
%option c++
%option yyclass="lexer"
%option prefix="cplus_"

%{
#include "lexer.h"
#include "shell.hpp"
#include "parser.hpp"

#define RESET   "\033[0m"
#define YELLOW  "\033[33m"
%}

digit      [0-9]
alpha      [a-zA-Z_]
alphanum   [a-zA-Z_0-9]

%%

#.*     ;       /* ignore comments */

[ \t\n] ;       /* ignore whitespaces */

"var" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: VAR" << RESET << std::endl;
    return cplus::parser::make_VAR();
}

"is" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: IS" << RESET << std::endl;
    return cplus::parser::make_IS();
}

"type" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: TYPE_KW" << RESET << std::endl;
    return cplus::parser::make_TYPE_KW();
}

"integer" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: INT_KW" << RESET << std::endl;
    return cplus::parser::make_INT_KW();
}

"real" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: REAL_KW" << RESET << std::endl;
    return cplus::parser::make_REAL_KW();
}

"boolean" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: BOOL_KW" << RESET << std::endl;
    return cplus::parser::make_BOOL_KW();
}

"array" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: ARRAY" << RESET << std::endl;
    return cplus::parser::make_ARRAY();
}

"record" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: RECORD" << RESET << std::endl;
    return cplus::parser::make_RECORD();
}

"routine" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: ROUTINE" << RESET << std::endl;
    return cplus::parser::make_ROUTINE();
}

"return" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: RETURN" << RESET << std::endl;
    return cplus::parser::make_RETURN();
}

"end" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: END" << RESET << std::endl;
    return cplus::parser::make_END();
}

"print" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: PRINT" << RESET << std::endl;
    return cplus::parser::make_PRINT();
}

"if" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: IF" << RESET << std::endl;
    return cplus::parser::make_IF();
}

"then" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: THEN" << RESET << std::endl;
    return cplus::parser::make_THEN();
}

"else" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: ELSE" << RESET << std::endl;
    return cplus::parser::make_ELSE();
}

"while" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: WHILE" << RESET << std::endl;
    return cplus::parser::make_WHILE();
}

"for" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: FOR" << RESET << std::endl;
    return cplus::parser::make_FOR();
}

"in" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: IN" << RESET << std::endl;
    return cplus::parser::make_IN();
}

"loop" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: LOOP" << RESET << std::endl;
    return cplus::parser::make_LOOP();
}

"reverse" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: REVERSE" << RESET << std::endl;
    return cplus::parser::make_REVERSE();
}

"and" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: AND" << RESET << std::endl;
    return cplus::parser::make_AND();
}

"or" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: OR" << RESET << std::endl;
    return cplus::parser::make_OR();
}

"xor" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: XOR" << RESET << std::endl;
    return cplus::parser::make_XOR();
}

"not" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: NOT" << RESET << std::endl;
    return cplus::parser::make_NOT();
}

"+" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: PLUS" << RESET << std::endl;
    return cplus::parser::make_PLUS();
}

"-" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: MINUS" << RESET << std::endl;
    return cplus::parser::make_MINUS();
}

"*" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: MUL" << RESET << std::endl;
    return cplus::parser::make_MUL();
}

"/" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: DIV" << RESET << std::endl;
    return cplus::parser::make_DIV();
}

"%" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: MOD" << RESET << std::endl;
    return cplus::parser::make_MOD();
}

";" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: SEMICOLON" << RESET << std::endl;
    return cplus::parser::make_SEMICOLON();
}

":" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: COLON" << RESET << std::endl;
    return cplus::parser::make_COLON();
}

"," {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: COMMA" << RESET << std::endl;
    return cplus::parser::make_COMMA();
}

"(" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: B_L" << RESET << std::endl;
    return cplus::parser::make_B_L();
}

")" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: B_R" << RESET << std::endl;
    return cplus::parser::make_B_R();
}

"[" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: SB_L" << RESET << std::endl;
    return cplus::parser::make_SB_L();
}

"]" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: SB_R" << RESET << std::endl;
    return cplus::parser::make_SB_R();
}

"{" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: CB_L" << RESET << std::endl;
    return cplus::parser::make_CB_L();
}

"}" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: CB_R" << RESET << std::endl;
    return cplus::parser::make_CB_R();
}

"." {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: DOT" << RESET << std::endl;
    return cplus::parser::make_DOT();
}

".." {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: DDOT" << RESET << std::endl;
    return cplus::parser::make_DDOT();
}

":=" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: BECOMES" << RESET << std::endl;
    return cplus::parser::make_BECOMES();
}

"=" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: EQ" << RESET << std::endl;
    return cplus::parser::make_EQ();
}

"<" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: LT" << RESET << std::endl;
    return cplus::parser::make_LT();
}

"<=" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: LEQ" << RESET << std::endl;
    return cplus::parser::make_LEQ();
}

">" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: GT" << RESET << std::endl;
    return cplus::parser::make_GT();
}

">=" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: GEQ" << RESET << std::endl;
    return cplus::parser::make_GEQ();
}

"/=" {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: NEQ" << RESET << std::endl;
    return cplus::parser::make_NEQ();
}

true {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: BOOL_VAL" << RESET << std::endl;
    return cplus::parser::make_BOOL_VAL(true);
}

false {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: BOOL_VAL" << RESET << std::endl;
    return cplus::parser::make_BOOL_VAL(false);
}

{alpha}{alphanum}* {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: IDENTIFIER" << RESET << std::endl;
    return cplus::parser::make_ID(yytext);
}

-?{digit}+\.{digit}+ {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: REAL_VAL" << RESET << std::endl;
    return cplus::parser::make_REAL_VAL(atof(yytext));
}

-?{digit}+ {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: INT_VAL" << RESET << std::endl;
    return cplus::parser::make_INT_VAL(atoi(yytext));
}

. {
    std::cout << YELLOW << "[LEXER]: Unknown token: " << yytext << '\n';
}

<<EOF>> {
    if (driver.debug) std::cout << YELLOW << "[LEXER]: EOF " << RESET << std::endl;
    return 0;
}

%%
