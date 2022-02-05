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

%token VAR ID IS INT_VAL REAL_VAL BOOL_VAL    // var <identifier> is \d+ \d+\.\d+ true|false
%token TYPE INT_KW REAL_KW BOOLEAN_KW         // type integer real boolean
%token B_L B_R SB_L SB_R CB_L CB_R            // ( ) [ ] { }
%token COLON SEMICOLON COMMA DOT DDOT BECOMES // : ; , . .. :=
%token PLUS MINUS MUL DIV MOD                 // + - * / %
%token AND OR XOR NOT                         // and or xor not
%token LT GT EQ LEQ GEQ NEQ                   // < > = <= >= /=
%token ARRAY RECORD ROUTINE RETURN END        // array record routine return end
%token PRINT                                  // print
%token IF THEN ELSE WHILE FOR IN LOOP REVERSE // if then else while for in loop reverse

%type <std::string> ID Type PrimitiveType UserType ArrayType RecordType
%type <int> INT_VAL INT_EXP
%type <double> REAL_VAL REAL_EXP
%type <bool> BOOL_VAL BOOL_EXP

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

// Used for constructing semicolon separated items
SemicolonSeparator : SEMICOLON | %empty
;

// Used for constructing comma separated items
CommaSeparator : COMMA | %empty
;

// TODO: PRINT should belong to statement grammar
Program:
    %empty { if (driver.pdebug) cout << "[PARSER]: EOF\n"; }
    | SimpleDeclaration SemicolonSeparator Program
    | RoutineDeclaration SemicolonSeparator Program
;

SimpleDeclaration:
    VariableDeclaration {
        program->variables.push_back($1);
        driver.prompt();
    }
    | TypeDeclaration { driver.prompt(); }
;

VariableDeclaration:
    VAR ID IS INT_EXP SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: int " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("integer", $2, $4);
    }
    | VAR ID IS REAL_EXP SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: real " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("real", $2, $4);
    }
    | VAR ID IS BOOL_EXP SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: boolean " << $2 << " = " << $4 << "\n";
        $$ = make_shared<Variable> ("boolean", $2, $4);
    }
    | VAR ID COLON Type SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << "\n";
        $$ = make_shared<Variable> ($4, $2, "null");
    }
    | VAR ID COLON Type IS Expression SEMICOLON {
        // TODO: replace 
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << " = " << "<Result>" << "\n";
        // TODO: there might be a type mismatch error here.
        $$ = make_shared<Variable> ("integer", $2, "<Result>");
    }
;

// TODO: Support simplification of expressions to one of the three primitive types

Expression :
    INT_EXP
    | REAL_EXP
    | BOOL_EXP
;

INT_EXP :
    INT_VAL
;

REAL_EXP :
    REAL_VAL
;

BOOL_EXP :
    BOOL_VAL
;


TypeDeclaration : TYPE ID IS Type SEMICOLON {
    if (driver.pdebug) cout << "[PARSER]: alias " << $2 << " for type " << $4 << "\n";
}
;

Type : PrimitiveType | UserType | ID
;

PrimitiveType : INT_KW { $$ = "integer"; }
                | REAL_KW { $$ = "real"; }
                | BOOLEAN_KW { $$ = "boolean"; }
;

UserType : ArrayType | RecordType
;

ArrayType : ARRAY SB_L INT_EXP SB_R Type { $$ = $5 + "[" + to_string($3) + "]"; }
;

RecordType : RECORD CB_L VariableDeclarations CB_R END { $$ = "record"; }
;

VariableDeclarations: %empty | VariableDeclaration SemicolonSeparator VariableDeclarations
;

RoutineDeclaration :
    ROUTINE ID B_L Parameters B_R IS Body END {
        if (driver.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
    }
    | ROUTINE ID B_L Parameters B_R COLON Type IS Body END {
        if (driver.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
    }
;

Parameters :
    %empty | ParameterDeclaration CommaSeparator Parameters
;

ParameterDeclaration :
    ID COLON ID
;

Body : %empty
    | SimpleDeclaration SemicolonSeparator Body
    | Statement SemicolonSeparator Body
;

Statement : Assignment | RoutineCall | WhileLoop | ForLoop | IfStatement | ReturnStatement | PrintStatement
;

Assignment : ModifiablePrimary BECOMES Expression {
    if (driver.pdebug) cout << "[PARSER]: Assignment statement parsed\n";
}
;

ModifiablePrimary :
    ID DOT ID
    | ID SB_L INT_EXP SB_R
    | ID

RoutineCall : ID B_L Expressions B_R {
    if (driver.pdebug) cout << "[PARSER]: routine call for " << $1 << " parsed\n";
}
;

Expressions :
    %empty | Expression CommaSeparator Expressions
;

WhileLoop : WHILE Expression LOOP Body END {
    if (driver.pdebug) cout << "[PARSER]: while loop parsed\n";
}
;

ForLoop :
    FOR ID IN Range LOOP Body END {
        if (driver.pdebug) cout << "[PARSER]: for loop parsed\n";
    }
    | FOR ID IN REVERSE Range LOOP Body END {
        if (driver.pdebug) cout << "[PARSER]: reverse for loop parsed\n";
    }
;

Range :
    Expression DDOT Expression
;

IfStatement :
    IF Expression THEN Body END {
        if (driver.pdebug) cout << "[PARSER]: if statement parsed\n";
    }
    | IF Expression THEN Body ELSE Body END {
        if (driver.pdebug) cout << "[PARSER]: if-else statement parsed\n";
    }
;

ReturnStatement :
    RETURN ID SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: return statement parsed\n";
    }
;

// Print statements are allowed only inside routines.
// For now, if a print statement was encountered in a routine declaration,
// it will be executed immediately, this can help with debugging for now.

// Future behaviour: program starts from "main" routine.
// Other routines should only be stored and executed only upon call. 

PrintStatement :
    PRINT ID SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: print " << $2 << "\n";
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
    | PRINT Expression SEMICOLON // TODO
    | PRINT RoutineCall SEMICOLON // TODO
;

%%
void cplus::parser::error(const std::string& msg)
{
    std::cout << msg << '\n';
}
