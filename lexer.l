%option noyywrap
%option nounistd
%option c++
%option yyclass="Lexer"
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

[\r\n]+ {
    LDEBUG("\n")
}

"var" {
    LDEBUG("VAR")
    return cplus::Parser::make_VAR();
}

"is" {
    LDEBUG("IS")
    return cplus::Parser::make_IS();
}

"type" {
    LDEBUG("TYPE")
    return cplus::Parser::make_TYPE_KW();
}

"integer" {
    LDEBUG("INT_KW")
    return cplus::Parser::make_INT_KW();
}

"real" {
    LDEBUG("REAL_KW")
    return cplus::Parser::make_REAL_KW();
}

"boolean" {
    LDEBUG("BOOL_KW")
    return cplus::Parser::make_BOOL_KW();
}

"array" {
    LDEBUG("ARRAY")
    return cplus::Parser::make_ARRAY();
}

"record" {
    LDEBUG("RECORD")
    return cplus::Parser::make_RECORD();
}

"routine" {
    LDEBUG("ROUTINE")
    return cplus::Parser::make_ROUTINE();
}

"return" {
    LDEBUG("RETURN")
    return cplus::Parser::make_RETURN();
}

"end" {
    LDEBUG("END")
    return cplus::Parser::make_END();
}

"print" {
    LDEBUG("PRINT")
    return cplus::Parser::make_PRINT();
}

"println" {
    LDEBUG("PRINTLN")
    return cplus::Parser::make_PRINTLN();
}

"if" {
    LDEBUG("IF")
    return cplus::Parser::make_IF();
}

"then" {
    LDEBUG("THEN")
    return cplus::Parser::make_THEN();
}

"else" {
    LDEBUG("ELSE")
    return cplus::Parser::make_ELSE();
}

"while" {
    LDEBUG("WHILE")
    return cplus::Parser::make_WHILE();
}

"for" {
    LDEBUG("FOR")
    return cplus::Parser::make_FOR();
}

"in" {
    LDEBUG("IN")
    return cplus::Parser::make_IN();
}

"loop" {
    LDEBUG("LOOP")
    return cplus::Parser::make_LOOP();
}

"reverse" {
    LDEBUG("REVERSE")
    return cplus::Parser::make_REVERSE();
}

"and" {
    LDEBUG("AND")
    return cplus::Parser::make_AND();
}

"or" {
    LDEBUG("OR")
    return cplus::Parser::make_OR();
}

"xor" {
    LDEBUG("XOR")
    return cplus::Parser::make_XOR();
}

"not" {
    LDEBUG("NOT")
    return cplus::Parser::make_NOT();
}

"+" {
    LDEBUG("PLUS")
    return cplus::Parser::make_PLUS();
}

"-" {
    LDEBUG("MINUS")
    return cplus::Parser::make_MINUS();
}

"*" {
    LDEBUG("MUL")
    return cplus::Parser::make_MUL();
}

"/" {
    LDEBUG("DIV")
    return cplus::Parser::make_DIV();
}

"%" {
    LDEBUG("MOD")
    return cplus::Parser::make_MOD();
}

";" {
    LDEBUG("SEMICOLON")
    return cplus::Parser::make_SEMICOLON();
}

":" {
    LDEBUG("COLON")
    return cplus::Parser::make_COLON();
}

"," {
    LDEBUG("COMMA")
    return cplus::Parser::make_COMMA();
}

"(" {
    LDEBUG("B_L")
    return cplus::Parser::make_B_L();
}

")" {
    LDEBUG("B_R")
    return cplus::Parser::make_B_R();
}

"[" {
    LDEBUG("SB_L")
    return cplus::Parser::make_SB_L();
}

"]" {
    LDEBUG("SB_R")
    return cplus::Parser::make_SB_R();
}

"{" {
    LDEBUG("CB_L")
    return cplus::Parser::make_CB_L();
}

"}" {
    LDEBUG("CB_R")
    return cplus::Parser::make_CB_R();
}

".." {
    LDEBUG("DDOT")
    return cplus::Parser::make_DDOT();
}

":=" {
    LDEBUG("BECOMES")
    return cplus::Parser::make_BECOMES();
}

"=" {
    LDEBUG("EQ")
    return cplus::Parser::make_EQ();
}

"<" {
    LDEBUG("LT")
    return cplus::Parser::make_LT();
}

"<=" {
    LDEBUG("LEQ")
    return cplus::Parser::make_LEQ();
}

">" {
    LDEBUG("GT")
    return cplus::Parser::make_GT();
}

">=" {
    LDEBUG("GEQ")
    return cplus::Parser::make_GEQ();
}

"/=" {
    LDEBUG("NEQ")
    return cplus::Parser::make_NEQ();
}

"->" {
	LDEBUG("ARROW")
	return cplus::Parser::make_ARROW();
}

true {
    LDEBUG("BOOL_VAL")
    return cplus::Parser::make_BOOL_VAL(true);
}

false {
    LDEBUG("BOOL_VAL")
    return cplus::Parser::make_BOOL_VAL(false);
}

{alpha}{alphanum}* {
    LDEBUG("ID")
    return cplus::Parser::make_ID(yytext);
}

-?{digit}+\.{digit}+ {
    LDEBUG("REAL_VAL")
    return cplus::Parser::make_REAL_VAL(atof(yytext));
}

-?{digit}+ {
    LDEBUG("INT_VAL")
    return cplus::Parser::make_INT_VAL(atoi(yytext));
}

"\"".*"\"" {
    LDEBUG("STRING")
    return cplus::Parser::make_STRING(yytext);
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
