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

%token VAR ID IS INT_VAL REAL_VAL BOOLEAN_VAL // var id is \d+ \d+\.\d+ true|false
%token TYPE INT_KW REAL_KW BOOLEAN_KW         // type integer real boolean
%token B_L B_R SB_L SB_R CB_L CB_R            // ( ) [ ] { }
%token COLON SEMICOLON COMMA                  // : ; ,
%token ARRAY RECORD ROUTINE END               // array record routine end
%token PRINT

%type <std::string> ID Type PrimitiveType UserType ArrayType RecordType
%type <int> INT_VAL
%type <double> REAL_VAL
%type <bool> BOOLEAN_VAL

%type <ast::ast_node<ast::Variable> > VariableDeclaration

%start Program

%code requires
{
    #include <iostream>
    #include <string>
    #include "ast.hpp"

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
    #include "shell.hpp"
    static cplus::parser::symbol_type yylex(cplus::lexer &lexer, cplus::shell &driver) {
        return lexer.get_next_token();
    }
    
    using namespace cplus;
    using namespace ast;
    
    ast_node<Program> program = make_shared<Program>();  // Points to the whole program node.
}


%%

// TODO: PRINT should belong to statement grammar
Program: %empty { if (driver.pdebug) cout << "[PARSER]: EOF\n"; }
        | SimpleDeclaration Separator Program
        | PRINT ID Separator Program {
            for(auto u : program->variables) {
                if(u->name == $2) {
                    if(u->dtype == "integer") {
                        cout << u->int_val << '\n';
                    }
                    else if(u->dtype == "real") {
                        cout << u->real_val << '\n';
                    }
                    else if(u->dtype == "boolean") {
                        cout << u->bool_val << '\n';
                    }
                }
            }
        }

;

Separator:  SEMICOLON | %empty
;

SimpleDeclaration:
    VariableDeclaration {
        program->variables.push_back($1);
        driver.prompt();
    }
    | TypeDeclaration { driver.prompt(); }
;

// TODO: Explicit matching for INT_VAL, etc. may be replaced with INT_Expression, etc.
// TODO: These rules can be reduced.
VariableDeclaration:
    VAR ID IS INT_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: int " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("integer", $2, $4);
    }
    | VAR ID IS REAL_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: real " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("real", $2, $4);
    }
    | VAR ID IS BOOLEAN_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: boolean " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("boolean", $2, $4);
    }
    | VAR ID COLON Type SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << "\n";
        $$ = make_shared<Variable> ($4, $2, "null");
    }
    | VAR ID COLON Type IS INT_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << " = " << $6 << "\n";
        // TODO: there might be a type mismatch error here.
        $$ = make_shared<Variable> ("integer", $2, $6);
    }
    | VAR ID COLON Type IS REAL_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << " = " << $6 << "\n";
        // TODO: there might be a type mismatch error here.
        $$ = make_shared<Variable> ("real", $2, $6);
    }
    | VAR ID COLON Type IS BOOLEAN_VAL SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << " = " << $6 << "\n";
        // TODO: there might be a type mismatch error here.
        $$ = make_shared<Variable> ("boolean", $2, $6);
    }
;

TypeDeclaration : TYPE ID IS Type SEMICOLON { if (driver.pdebug) cout << "[PARSER]: alias " << $2 << " for type " << $4 << "\n"; }
;

Type : PrimitiveType | UserType | ID
;

PrimitiveType : INT_KW { $$ = "integer"; }
                | REAL_KW { $$ = "real"; }
                | BOOLEAN_KW { $$ = "boolean"; }
;

UserType : ArrayType | RecordType
;

// Explicit matching for INT_VAL may be replaced with INT_Expression
ArrayType : ARRAY SB_L INT_VAL SB_R Type { $$ = $5 + "[" + to_string($3) + "]"; }
;

RecordType : RECORD CB_L VariableDeclarations CB_R END { $$ = "record"; }
;

VariableDeclarations: %empty | VariableDeclaration Separator VariableDeclarations
;



%%
void cplus::parser::error(const std::string& msg)
{
    std::cout << msg << '\n';
}
