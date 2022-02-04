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

%token VAR ID IS INT_VAL REAL_VAL BOOLEAN_VAL SEMICOLON
%token TYPE INT_KW REAL_KW BOOLEAN_KW

%type <std::string> ID
%type <std::string> Type
%type <std::string> PrimitiveType

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

SimpleDeclaration: VariableDeclaration { if(driver.interactive) std::cout << "cplus> "; }
                    | TypeDeclaration { if(driver.interactive) std::cout << "cplus> "; }
;

VariableDeclaration:
    VAR ID IS INT_VAL SEMICOLON { std::cout << "[PARSER]: integer variable " << $2 << "\n"; }
    | VAR ID IS REAL_VAL SEMICOLON { std::cout << "[PARSER]: real variable " << $2 << "\n"; }
    | VAR ID IS BOOLEAN_VAL SEMICOLON { std::cout << "[PARSER]: boolean variable " << $2 << "\n"; }
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
