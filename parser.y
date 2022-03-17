%language "C++"
%skeleton "lalr1.cc"
%require "3.2"

%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define api.parser.class { Parser }
%define api.namespace    { cplus }

%lex-param   { cplus::Lexer &lexer }
%lex-param   { cplus::Shell &shell }
%parse-param { cplus::Lexer &lexer }
%parse-param { cplus::Shell &shell }

%token VAR ID IS INT_VAL REAL_VAL BOOL_VAL    // var <identifier> is \d+ \d+\.\d+ true|false
%token TYPE_KW INT_KW REAL_KW BOOL_KW         // type integer real boolean
%token B_L B_R SB_L SB_R CB_L CB_R            // ( ) [ ] { }
%token COLON SEMICOLON COMMA DDOT BECOMES     // : ; , . .. :=
%token PLUS MINUS MUL DIV MOD                 // + - * / %
%token AND OR XOR NOT                         // and or xor not
%token LT GT EQ LEQ GEQ NEQ                   // < > = <= >= /=
%token ARRAY RECORD ROUTINE RETURN END        // array record routine return end
%token PRINT PRINTLN STRING                   // print println <string>
%token IF THEN ELSE WHILE FOR IN LOOP REVERSE // if then else while for in loop reverse

%type <std::string> ID STRING
%type <long long> INT_VAL
%type <double> REAL_VAL
%type <bool> BOOL_VAL

%type <ast::node_ptr<ast::VariableDeclaration>> VARIABLE_DECLARATION PARAMETER_DECLARATION
%type <std::vector<ast::node_ptr<ast::VariableDeclaration>>> VARIABLE_DECLARATIONS
%type <std::vector<ast::node_ptr<ast::VariableDeclaration>>> PARAMETERS NON_EMPTY_PARAMETERS 
%type <ast::node_ptr<ast::RoutineDeclaration>> ROUTINE_DECLARATION
%type <ast::node_ptr<ast::Expression>> EXPRESSION
%type <std::vector<ast::node_ptr<ast::Expression>>> EXPRESSIONS NON_EMPTY_EXPRESSIONS
%type <ast::node_ptr<ast::Type>> TYPE PRIMITIVE_TYPE ARRAY_TYPE RECORD_TYPE
%type <ast::node_ptr<ast::Body>> BODY
%type <ast::node_ptr<ast::Identifier>> MODIFIABLE_PRIMARY
%type <ast::node_ptr<ast::Statement>> STATEMENT
%type <ast::node_ptr<ast::ReturnStatement>> RETURN_STATEMENT
%type <ast::node_ptr<ast::PrintStatement>> PRINT_STATEMENT
%type <ast::node_ptr<ast::AssignmentStatement>> ASSIGNMENT_STATEMENT
%type <ast::node_ptr<ast::IfStatement>> IF_STATEMENT
%type <ast::node_ptr<ast::WhileLoop>> WHILE_LOOP
%type <ast::node_ptr<ast::ForLoop>> FOR_LOOP
%type <ast::node_ptr<ast::RoutineCall>> ROUTINE_CALL

%left COMMA
%right BECOMES
%left OR
%left AND
%left EQ NEQ XOR
%left LT LEQ GT GEQ
%left PLUS MINUS
%left MUL DIV MOD
%right NOT

%start PROGRAM

%code requires {
    #include <iostream>
    #include <string>
    #include <vector>
    #include "ast.hpp"

    namespace cplus {
    class Lexer;
    class Shell;
    }
}

%code top {
    #include "lexer.h"
    #include "shell.hpp"

    #define RESET   "\033[0m"
    #define GREEN   "\033[32m"
    #define PDEBUG(X) if (shell.debug) std::cout << GREEN << "(" << X << ") " << RESET;

    static cplus::Parser::symbol_type yylex(cplus::Lexer &lexer, cplus::Shell &shell) {
        return lexer.get_next_token();
    }
    
    ast::node_ptr<ast::Program> program = std::make_shared<ast::Program>();  // Points to the whole program node.
}


%%

PROGRAM :
    %empty {
        PDEBUG("EOF")
        std::cout << '\n' << std::endl;
    }
    | VARIABLE_DECLARATION PROGRAM {
        program->variables.push_back($1);
    }
    | ROUTINE_DECLARATION PROGRAM
    | GLOBAL_TYPE_DECLARATION PROGRAM
;

VARIABLE_DECLARATION :
    VAR ID IS EXPRESSION SEMICOLON {
        PDEBUG("VARIABLE_DECLARATION_W/O_TYPE")
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4);
    }
    | VAR ID COLON TYPE SEMICOLON {
        PDEBUG("VARIABLE_DECLARATION_W/O_IV")
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4);
    }
    | VAR ID COLON TYPE IS EXPRESSION SEMICOLON {
        PDEBUG("VARIABLE_DECLARATION")
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4, $6);
    }
;

GLOBAL_TYPE_DECLARATION :
    TYPE_KW ID IS TYPE SEMICOLON {
        PDEBUG("GLOBAL_TYPE_DECLARATION")
        program->types[$2] = $4;
    }
;

MODIFIABLE_PRIMARY :
    ID                                { $$ = std::make_shared<ast::Identifier>($1); }
    | ID SB_L EXPRESSION SB_R         { $$ = std::make_shared<ast::Identifier>($1, $3); }
;

EXPRESSION :
    INT_VAL                           { $$ = std::make_shared<ast::IntLiteral>($1); }
    | REAL_VAL                        { $$ = std::make_shared<ast::RealLiteral>($1); }
    | BOOL_VAL                        { $$ = std::make_shared<ast::BoolLiteral>($1); }
    | ROUTINE_CALL                    { PDEBUG("ROUTINE_CALL_EXP") $$ = $1; }
    | B_L EXPRESSION B_R              { $$ = $2; }
    | NOT EXPRESSION                  { $$ = std::make_shared<ast::UnaryExpression>(ast::OperatorEnum::NOT, $2); }
    | MINUS EXPRESSION                { $$ = std::make_shared<ast::UnaryExpression>(ast::OperatorEnum::MINUS, $2); }
    | MODIFIABLE_PRIMARY              { $$ = $1; }
    
    | EXPRESSION PLUS EXPRESSION      { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::PLUS, $3); }
    | EXPRESSION MINUS EXPRESSION     { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::MINUS, $3); } 
    | EXPRESSION MUL EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::MUL, $3); } 
    | EXPRESSION DIV EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::DIV, $3); }
    | EXPRESSION MOD EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::MOD, $3); }
    | EXPRESSION AND EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::AND, $3); }
    | EXPRESSION OR EXPRESSION        { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::OR, $3); }
    | EXPRESSION XOR EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::XOR, $3); }
    | EXPRESSION EQ EXPRESSION        { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::EQ, $3); } 
    | EXPRESSION NEQ EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::NEQ, $3); } 
    | EXPRESSION LT EXPRESSION        { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::LT, $3); } 
    | EXPRESSION GT EXPRESSION        { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::GT, $3); } 
    | EXPRESSION LEQ EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::LEQ, $3); } 
    | EXPRESSION GEQ EXPRESSION       { $$ = std::make_shared<ast::BinaryExpression>($1, ast::OperatorEnum::GEQ, $3); } 
;

TYPE :
    PRIMITIVE_TYPE
    | ARRAY_TYPE
    | RECORD_TYPE
    | ID {
        PDEBUG("ALIASED_TYPE_ACCESS")
        $$ = program->types[$1];
    }
;

PRIMITIVE_TYPE :
    INT_KW    { $$ = std::make_shared<ast::IntType>(); }
    | REAL_KW { $$ = std::make_shared<ast::RealType>(); }
    | BOOL_KW { $$ = std::make_shared<ast::BoolType>(); }
;

ARRAY_TYPE :
    ARRAY SB_L EXPRESSION SB_R TYPE {
        PDEBUG("ARRAY_TYPE")
        $$ = std::make_shared<ast::ArrayType>($3, $5);
    }
;

RECORD_TYPE :
    RECORD CB_L VARIABLE_DECLARATIONS CB_R END {
        PDEBUG("RECORD_TYPE")
        $$ = std::make_shared<ast::RecordType>($3);
    }
;

VARIABLE_DECLARATIONS :
    VARIABLE_DECLARATION {
        $$ = std::vector<ast::node_ptr<ast::VariableDeclaration>>(1, $1);
    }
    | VARIABLE_DECLARATION VARIABLE_DECLARATIONS {
        $2.push_back($1);
        $$ = $2;
    }
;

ROUTINE_DECLARATION :
    ROUTINE ID B_L PARAMETERS B_R IS BODY END {
        PDEBUG("PROCEDURE_DECLARATION")
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $7);
        program->routines.push_back($$);
    }
    | ROUTINE ID B_L PARAMETERS B_R COLON TYPE IS BODY END {
        PDEBUG("FUNCTION_DECLARATION")
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $9, $7);
        program->routines.push_back($$);
    }
;


NON_EMPTY_PARAMETERS :
    PARAMETER_DECLARATION {
        $$ = std::vector<ast::node_ptr<ast::VariableDeclaration>>(1, $1);
    }
    | PARAMETER_DECLARATION COMMA NON_EMPTY_PARAMETERS {
        $3.push_back($1);
        $$ = $3;
    }
;

PARAMETERS :
    %empty {
        $$ = std::vector<ast::node_ptr<ast::VariableDeclaration>>();
    }
    | NON_EMPTY_PARAMETERS {
        $$ = $1;
    }
;

PARAMETER_DECLARATION :
    ID COLON TYPE {
        PDEBUG("PARAMETER_DECLARATION")
        $$ = std::make_shared<ast::VariableDeclaration>($1, $3);
    }
;

BODY :
    %empty {
        std::vector<ast::node_ptr<ast::VariableDeclaration>> tmp1;
        std::vector<ast::node_ptr<ast::Statement>> tmp2;
        $$ = std::make_shared<ast::Body>(tmp1, tmp2);
    }
    | VARIABLE_DECLARATION BODY {
        $2->variables.push_back($1);
        $$ = $2;
    }
    | STATEMENT BODY {
        $2->statements.push_back($1);
        $$ = $2;
    }
;

STATEMENT :
    RETURN_STATEMENT         { $$ = $1; }
    | PRINT_STATEMENT        { $$ = $1; }
    | ASSIGNMENT_STATEMENT   { $$ = $1; }
    | IF_STATEMENT           { $$ = $1; }
    | WHILE_LOOP             { $$ = $1; }
    | FOR_LOOP               { $$ = $1; }
    | ROUTINE_CALL SEMICOLON {
        PDEBUG("ROUTINE_CALL_STMT")
        $$ = $1;
    }
;

RETURN_STATEMENT :
    RETURN EXPRESSION SEMICOLON {
        PDEBUG("RETURN_EXP_STATEMENT")
        $$ = std::make_shared<ast::ReturnStatement>($2);
    }
    | RETURN SEMICOLON {
        PDEBUG("RETURN_STATEMENT")
        $$ = std::make_shared<ast::ReturnStatement>();
    }
;

PRINT_STATEMENT :
    PRINT EXPRESSION SEMICOLON {
        PDEBUG("PRINT_EXP_STATEMENT")
        $$ = std::make_shared<ast::PrintStatement>($2);
    }
    | PRINT STRING SEMICOLON {
        PDEBUG("PRINT_STR_STATEMENT")
        $2 = $2.substr(1, $2.size()-2);
        $$ = std::make_shared<ast::PrintStatement>(std::make_shared<std::string>($2));
    }
    | PRINTLN EXPRESSION SEMICOLON {
        PDEBUG("PRINTLN_EXP_STATEMENT")
        $$ = std::make_shared<ast::PrintStatement>($2, true);
    }
    | PRINTLN STRING SEMICOLON {
        PDEBUG("PRINTLN_STR_STATEMENT")
        $2 = $2.substr(1, $2.size()-2);
        $$ = std::make_shared<ast::PrintStatement>(std::make_shared<std::string>($2), true);
    }
;

ASSIGNMENT_STATEMENT :
    MODIFIABLE_PRIMARY BECOMES EXPRESSION SEMICOLON {
        PDEBUG("ASSIGNMENT_STATEMENT")
        $$ = std::make_shared<ast::AssignmentStatement>($1, $3);
    }
;

IF_STATEMENT :
    IF EXPRESSION THEN BODY END {
        PDEBUG("IF_STATEMENT")
        $$ = std::make_shared<ast::IfStatement>($2, $4);
    }
    | IF EXPRESSION THEN BODY ELSE BODY END {
        PDEBUG("IF_ELSE_STATEMENT")
        $$ = std::make_shared<ast::IfStatement>($2, $4, $6);
    }
;

WHILE_LOOP :
    WHILE EXPRESSION LOOP BODY END {
        PDEBUG("WHILE_LOOP")
        $$ = std::make_shared<ast::WhileLoop>($2, $4);
    }
;

FOR_LOOP :
    FOR ID IN EXPRESSION DDOT EXPRESSION LOOP BODY END {
        PDEBUG("FOR_LOOP")

        auto loop_var = std::make_shared<ast::VariableDeclaration>($2, $4);
        auto id = std::make_shared<ast::Identifier>($2);
        auto one = std::make_shared<ast::IntLiteral>(1);
        auto idp1 = std::make_shared<ast::BinaryExpression>(id, ast::OperatorEnum::PLUS, one);
        auto cond = std::make_shared<ast::BinaryExpression>(id, ast::OperatorEnum::LEQ, $6);
        auto body = $8;
        auto action = std::make_shared<ast::AssignmentStatement>(id, idp1);

        $$ = std::make_shared<ast::ForLoop>(loop_var, cond, body, action);
    }
    | FOR ID IN REVERSE EXPRESSION DDOT EXPRESSION LOOP BODY END {
        PDEBUG("FOR_REVERSE_LOOP")

        auto loop_var = std::make_shared<ast::VariableDeclaration>($2, $7);
        auto id = std::make_shared<ast::Identifier>($2);
        auto one = std::make_shared<ast::IntLiteral>(1);
        auto idm1 = std::make_shared<ast::BinaryExpression>(id, ast::OperatorEnum::MINUS, one);
        auto cond = std::make_shared<ast::BinaryExpression>(id, ast::OperatorEnum::GEQ, $5);
        auto body = $9;
        auto action = std::make_shared<ast::AssignmentStatement>(id, idm1);

        $$ = std::make_shared<ast::ForLoop>(loop_var, cond, body, action);
    }
;

ROUTINE_CALL :
    ID B_L EXPRESSIONS B_R {
        ast::node_ptr<ast::RoutineCall> call;
        for(auto u : program->routines) {
            if(u->name == $1) {
                $$ = std::make_shared<ast::RoutineCall>(u, $3);
                break;        
            }
        }
    }
; 

NON_EMPTY_EXPRESSIONS :
    EXPRESSION {
        $$ = std::vector<ast::node_ptr<ast::Expression>>(1, $1);
    }
    | EXPRESSION COMMA NON_EMPTY_EXPRESSIONS {
        $3.push_back($1);
        $$ = $3;
    }
;


EXPRESSIONS :
    %empty {
        $$ = std::vector<ast::node_ptr<ast::Expression>>();
    }
    | NON_EMPTY_EXPRESSIONS {
        $$ = $1;
    }
;

%%
void cplus::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
