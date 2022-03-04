%option noyywrap
%option nounistd
%option c++
%option yyclass="lexer"
%option prefix="cplus_"

%{
#include "lexer.h"
#include "shell.hpp"
#include "parser.hpp"

#define RESET     "\033[0m"
#define YELLOW    "\033[33m"
#define LDEBUG(X) if (driver.debug) std::cout << YELLOW << X << RESET << " ";  
%}

digit      [0-9]
alpha      [a-zA-Z_]
alphanum   [a-zA-Z_0-9\.]

%%

#.* ; /* ignore comments */

[ \t]+ {
    LDEBUG(" ")
}

[\n]+ {
    LDEBUG("\n")
}

"var" {
    LDEBUG("VAR")
    return cplus::parser::make_VAR();
}

"is" {
    LDEBUG("IS")
    return cplus::parser::make_IS();
}

"type" {
    LDEBUG("TYPE")
    return cplus::parser::make_TYPE_KW();
}

"integer" {
    LDEBUG("INT")
    return cplus::parser::make_INT_KW();
}

"real" {
    LDEBUG("REAL")
    return cplus::parser::make_REAL_KW();
}

"boolean" {
    LDEBUG("BOOL")
    return cplus::parser::make_BOOL_KW();
}

"array" {
    LDEBUG("ARRAY")
    return cplus::parser::make_ARRAY();
}

"record" {
    LDEBUG("RECORD")
    return cplus::parser::make_RECORD();
}

"routine" {
    LDEBUG("ROUTINE")
    return cplus::parser::make_ROUTINE();
}

"return" {
    LDEBUG("RETURN")
    return cplus::parser::make_RETURN();
}

"end" {
    LDEBUG("END")
    return cplus::parser::make_END();
}

"print" {
    LDEBUG("PRINT")
    return cplus::parser::make_PRINT();
}

"if" {
    LDEBUG("IF")
    return cplus::parser::make_IF();
}

"then" {
    LDEBUG("THEN")
    return cplus::parser::make_THEN();
}

"else" {
    LDEBUG("ELSE")
    return cplus::parser::make_ELSE();
}

"while" {
    LDEBUG("WHILE")
    return cplus::parser::make_WHILE();
}

"for" {
    LDEBUG("FOR")
    return cplus::parser::make_FOR();
}

"in" {
    LDEBUG("IN")
    return cplus::parser::make_IN();
}

"loop" {
    LDEBUG("LOOP")
    return cplus::parser::make_LOOP();
}

"reverse" {
    LDEBUG("REVERSE")
    return cplus::parser::make_REVERSE();
}

"and" {
    LDEBUG("AND")
    return cplus::parser::make_AND();
}

"or" {
    LDEBUG("OR")
    return cplus::parser::make_OR();
}

"xor" {
    LDEBUG("XOR")
    return cplus::parser::make_XOR();
}

"not" {
    LDEBUG("NOT")
    return cplus::parser::make_NOT();
}

"+" {
    LDEBUG("PLUS")
    return cplus::parser::make_PLUS();
}

"-" {
    LDEBUG("MINUS")
    return cplus::parser::make_MINUS();
}

"*" {
    LDEBUG("MUL")
    return cplus::parser::make_MUL();
}

"/" {
    LDEBUG("DIV")
    return cplus::parser::make_DIV();
}

"%" {
    LDEBUG("MOD")
    return cplus::parser::make_MOD();
}

";" {
    LDEBUG("SEMICOLON")
    return cplus::parser::make_SEMICOLON();
}

":" {
    LDEBUG("COLON")
    return cplus::parser::make_COLON();
}

"," {
    LDEBUG("COMMA")
    return cplus::parser::make_COMMA();
}

"(" {
    LDEBUG("B_L")
    return cplus::parser::make_B_L();
}

")" {
    LDEBUG("B_R")
    return cplus::parser::make_B_R();
}

"[" {
    LDEBUG("SB_L")
    return cplus::parser::make_SB_L();
}

"]" {
    LDEBUG("SB_R")
    return cplus::parser::make_SB_R();
}

"{" {
    LDEBUG("CB_L")
    return cplus::parser::make_CB_L();
}

"}" {
    LDEBUG("CB_R")
    return cplus::parser::make_CB_R();
}

".." {
    LDEBUG("DDOT")
    return cplus::parser::make_DDOT();
}

":=" {
    LDEBUG("BECOMES")
    return cplus::parser::make_BECOMES();
}

"=" {
    LDEBUG("EQ")
    return cplus::parser::make_EQ();
}

"<" {
    LDEBUG("LT")
    return cplus::parser::make_LT();
}

"<=" {
    LDEBUG("LEQ")
    return cplus::parser::make_LEQ();
}

">" {
    LDEBUG("GT")
    return cplus::parser::make_GT();
}

">=" {
    LDEBUG("GEQ")
    return cplus::parser::make_GEQ();
}

"/=" {
    LDEBUG("NEQ")
    return cplus::parser::make_NEQ();
}

true {
    LDEBUG("BOOL_VAL")
    return cplus::parser::make_BOOL_VAL(true);
}

false {
    LDEBUG("BOOL_VAL")
    return cplus::parser::make_BOOL_VAL(false);
}

{alpha}{alphanum}* {
    LDEBUG("ID")
    return cplus::parser::make_ID(yytext);
}

-?{digit}+\.{digit}+ {
    LDEBUG("REAL_VAL")
    return cplus::parser::make_REAL_VAL(atof(yytext));
}

-?{digit}+ {
    LDEBUG("INT_VAL")
    return cplus::parser::make_INT_VAL(atoi(yytext));
}

<<EOF>> {
    LDEBUG("EOF")
    return 0;
}

. {
    std::cerr << YELLOW << "[LEXER]: Unknown token: " << yytext << '\n';
    std::exit(1);
}

%%
