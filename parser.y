%language "C++"
%skeleton "lalr1.cc"
%require "3.2"

%define api.value.type variant
%define api.token.constructor
%define parse.assert

%define api.parser.class { parser }
%define api.namespace { cplus }

%lex-param { cplus::lexer &lexer }
%lex-param { cplus::shell &driver }
%parse-param { cplus::lexer &lexer }
%parse-param { cplus::shell &driver }

%token VAR ID IS INT_VAL REAL_VAL BOOLEAN_VAL
%token TYPE INT_KW REAL_KW BOOLEAN_KW
%token COLON SEMICOLON

%type <std::string> ID
%type <std::string> Type
%type <std::string> PrimitiveType
%type <int> INT_VAL
%type <double> REAL_VAL
%type <bool> BOOLEAN_VAL

%start Program

%code requires
{
    #pragma once
    #include <iostream>
    #include <string>

    using namespace std;

    namespace cplus
    {
        class lexer;
        class shell;
    }
}

%code top
{
    #include "lexer.h"
    #include "parser.hpp"
    #include "shell.h"
    static cplus::parser::symbol_type yylex(cplus::lexer &lexer, cplus::shell &driver) {
        return lexer.get_next_token();
    }
    using namespace cplus;
}


%%

Program: %empty { std::cout << "[PARSER]: EOF\n"; }
        | SimpleDeclaration Separator Program

;

Separator:  SEMICOLON | %empty
;

SimpleDeclaration: VariableDeclaration { driver.prompt(); }
                    | TypeDeclaration { driver.prompt(); }
;

VariableDeclaration:
    VAR ID IS INT_VAL SEMICOLON { std::cout << "[PARSER]: int " << $2 << " = " << $4 << "\n"; }
    | VAR ID IS REAL_VAL SEMICOLON { std::cout << "[PARSER]: real " << $2 << " = " << $4 << "\n"; }
    | VAR ID IS BOOLEAN_VAL SEMICOLON { std::cout << "[PARSER]: boolean " << $2 << " = " << $4 << "\n"; }
    | VAR ID COLON Type SEMICOLON { std::cout << "[PARSER]: " << $4 << " " << $2 << "\n"; }
;

TypeDeclaration : TYPE ID IS Type SEMICOLON { std::cout << "[PARSER]: alias " << $2 << " for type " << $4 << "\n"; }
;

Type : PrimitiveType
;

PrimitiveType : INT_KW { $$ = "integer"; }
                | REAL_KW { $$ = "real"; }
                | BOOLEAN_KW { $$ = "boolean"; }
;

%%
void cplus::parser::error(const std::string& msg)
{
    std::cout << msg << '\n';
}
