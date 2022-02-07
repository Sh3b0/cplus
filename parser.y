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
%type <ast::ast_node<ast::ExpressionNode> > Expression

%left COMMA
%right BECOMES
%left OR
%left AND
%left EQ NEQ XOR
%left LT LEQ GT GEQ
%left PLUS MINUS
%left MUL DIV MOD
%right NOT
%left DOT

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
    VAR ID IS Expression SEMICOLON {
        auto exp = $4;
        if (driver.pdebug) cout << "[PARSER]: " << exp->dtype << " " << $2 << " = " << exp->value << "\n";
        $$ = make_shared<Variable> (exp->dtype, $2, make_shared<Literal>(exp->value));
    }
    | VAR ID COLON Type SEMICOLON {
        if (driver.pdebug) cout << "[PARSER]: " << $4 << " " << $2 << "\n";
        $$ = make_shared<Variable> ($4, $2, make_shared<Literal>());
    }
    | VAR ID COLON Type IS Expression SEMICOLON {
        auto exp = $6;
        if (driver.pdebug) cout << "[PARSER]: " << exp->dtype << " " << $2 << " = " << exp->value << "\n";
        
        if (exp->dtype != $4) {
            cout << "[PARSER]: Error: Type mismatch: " << exp->dtype << " cannot be unified with " << $4 << '\n';
        }

        $$ = make_shared<Variable> (exp->dtype, $2, make_shared<Literal>(exp->value));
    }
;

Expression :
    INT_EXP {
        if (driver.pdebug) cout << "[PARSER]: INT_EXP evaluates to " << $1 << "\n";
        $$ = make_shared<ExpressionNode>("integer", make_shared<Literal>($1));
    }
    | REAL_EXP {
        if (driver.pdebug) cout << "[PARSER]: REAL_EXP evaluates to " << $1 << "\n";
        $$ = make_shared<ExpressionNode>("real", make_shared<Literal>($1));
    }
    | BOOL_EXP {
        if (driver.pdebug) cout << "[PARSER]: BOOL_EXP evaluates to " << $1 << "\n";
        $$ = make_shared<ExpressionNode>("boolean", make_shared<Literal>($1));
    }
    // TODO: Expression can also be a ModifiablePrimary (e.g., a[0] + rec.item + 4 + x).
;

INT_EXP: INT_VAL				{ $$ = $1; }
	  | INT_EXP PLUS INT_EXP	{ $$ = $1 + $3; }
	  | INT_EXP MINUS INT_EXP	{ $$ = $1 - $3; }
	  | INT_EXP MUL INT_EXP     { $$ = $1 * $3; }
      | INT_EXP MOD INT_EXP     { $$ = $1 % $3; }
	  | B_L INT_EXP B_R		    { $$ = $2; }
;

REAL_EXP: REAL_VAL               { $$ = $1; }
    | REAL_EXP PLUS REAL_EXP     { $$ = $1 + $3; }
    | REAL_EXP MINUS REAL_EXP	 { $$ = $1 - $3; }
    | REAL_EXP MUL REAL_EXP      { $$ = $1 * $3; }
    | REAL_EXP DIV REAL_EXP      { $$ = $1 / $3; }
    | B_L REAL_EXP B_R           { $$ = $2; }
    | INT_EXP PLUS REAL_EXP      { $$ = $1 + $3; }
    | INT_EXP MINUS REAL_EXP     { $$ = $1 - $3; }
    | INT_EXP MUL REAL_EXP       { $$ = $1 * $3; }
    | INT_EXP DIV REAL_EXP       { $$ = $1 / $3; }
    | REAL_EXP PLUS INT_EXP      { $$ = $1 + $3; }
    | REAL_EXP MINUS INT_EXP     { $$ = $1 - $3; }
    | REAL_EXP MUL INT_EXP       { $$ = $1 * $3; }
    | REAL_EXP DIV INT_EXP       { $$ = $1 / $3; }
    | INT_EXP DIV INT_EXP        { $$ = $1 / (double)$3; }
;

BOOL_EXP : BOOL_VAL { $$ = $1; }
    | BOOL_EXP AND BOOL_EXP  { $$ = $1 && $3; }
    | BOOL_EXP OR BOOL_EXP   { $$ = $1 || $3; }
    | BOOL_EXP XOR BOOL_EXP  { $$ = $1 != $3; }
    | NOT BOOL_EXP           { $$ = !($2); }
    | B_L BOOL_EXP B_R       { $$ = $2; }
    | INT_EXP LT INT_EXP     { $$ = $1 < $3; }
    | INT_EXP LEQ INT_EXP    { $$ = $1 <= $3; }
    | INT_EXP GT INT_EXP     { $$ = $1 > $3; }
    | INT_EXP GEQ INT_EXP    { $$ = $1 >= $3; }
    | INT_EXP EQ INT_EXP     { $$ = ($1 == $3); }
    | INT_EXP NEQ INT_EXP    { $$ = ($1 != $3); }
    // TODO: add more rules for real values comparison (and hybrid).
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
    ID COLON Type
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
                cout << u->value << '\n';
                break;
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
