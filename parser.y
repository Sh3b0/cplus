%language "C++"
%skeleton "lalr1.cc"
%require "3.2"

%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define api.parser.class { parser }
%define api.namespace    { cplus }

%lex-param   { cplus::lexer &lexer }
%lex-param   { cplus::shell &shell }
%parse-param { cplus::lexer &lexer }
%parse-param { cplus::shell &shell }

%token VAR ID IS INT_VAL REAL_VAL BOOL_VAL    // var <identifier> is \d+ \d+\.\d+ true|false
%token TYPE_KW INT_KW REAL_KW BOOL_KW         // type integer real boolean
%token B_L B_R SB_L SB_R CB_L CB_R            // ( ) [ ] { }
%token COLON SEMICOLON COMMA DOT DDOT BECOMES // : ; , . .. :=
%token PLUS MINUS MUL DIV MOD                 // + - * / %
%token AND OR XOR NOT                         // and or xor not
%token LT GT EQ LEQ GEQ NEQ                   // < > = <= >= /=
%token ARRAY RECORD ROUTINE RETURN END        // array record routine return end
%token PRINT                                  // print
%token IF THEN ELSE WHILE FOR IN LOOP REVERSE // if then else while for in loop reverse

%type <std::string> ID
%type <long long> INT_VAL
%type <double> REAL_VAL
%type <bool> BOOL_VAL

%type <ast::np<ast::VariableDeclaration>> VARIABLE_DECLARATION PARAMETER_DECLARATION
%type <ast::np<ast::RoutineDeclaration>> ROUTINE_DECLARATION
%type <std::vector<ast::np<ast::VariableDeclaration>>> PARAMETERS
%type <ast::np<ast::Expression>> EXPRESSION
%type <ast::np<ast::Type>> TYPE PRIMITIVE_TYPE
%type <ast::np<ast::Body>> BODY
%type <ast::np<ast::Statement>> STATEMENT
%type <ast::np<ast::ReturnStatement>> RETURN_STATEMENT
%type <ast::np<ast::PrintStatement>> PRINT_STATEMENT
%type <ast::np<ast::AssignmentStatement>> ASSIGNMENT_STATEMENT

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

%start PROGRAM

%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include "ast.hpp"

    namespace cplus {
    class lexer;
    class shell;
    }
}

%code top
{
    #include "lexer.h"
    #include "shell.hpp"
    #define RESET   "\033[0m"
    #define GREEN   "\033[32m"

    static cplus::parser::symbol_type yylex(cplus::lexer &lexer, cplus::shell &shell) {
        return lexer.get_next_token();
    }
    
    ast::np<ast::Program> program = std::make_shared<ast::Program>();  // Points to the whole program node.
}


%%

// Used for constructing comma separated items
COMMA_SEPARATOR : COMMA | %empty
;

PROGRAM:
    %empty {
        if (shell.debug) std::cout << GREEN << "[PARSER]: EOF" << RESET << std::endl;
    }
    | VARIABLE_DECLARATION PROGRAM {
        if(shell.debug) std::cout << GREEN << "[PARSER]: VARABLE_DECLARATION" << RESET << std::endl;
        program->variables.push_back($1);
    }
    | ROUTINE_DECLARATION PROGRAM {
        program->routines.push_back($1);
    }
;

VARIABLE_DECLARATION:
    VAR ID IS EXPRESSION SEMICOLON {
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4);
    }

    | VAR ID COLON TYPE SEMICOLON {
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4);
    }

    | VAR ID COLON TYPE IS EXPRESSION SEMICOLON {
        $$ = std::make_shared<ast::VariableDeclaration> ($2, $4, $6);
    }
;

EXPRESSION :
    ID                                { $$ = std::make_shared<ast::Identifier>($1); }
    | INT_VAL                         { $$ = std::make_shared<ast::IntLiteral>($1); }
    | REAL_VAL                        { $$ = std::make_shared<ast::RealLiteral>($1); }
    | BOOL_VAL                        { $$ = std::make_shared<ast::BoolLiteral>($1); }

    | B_L EXPRESSION B_R              { $$ = $2; }
    | NOT EXPRESSION                  { $$ = std::make_shared<ast::UnaryExpression>(ast::OperatorEnum::NOT, $2); }
    | MINUS EXPRESSION                { $$ = std::make_shared<ast::UnaryExpression>(ast::OperatorEnum::MINUS, $2); }
    
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
;

PRIMITIVE_TYPE:
    INT_KW    { $$ = std::make_shared<ast::IntType>(); }
    | REAL_KW { $$ = std::make_shared<ast::RealType>(); }
    | BOOL_KW { $$ = std::make_shared<ast::BoolType>(); }
;

ROUTINE_DECLARATION :
    ROUTINE ID B_L PARAMETERS B_R IS BODY END {
        if (shell.debug) std::cout << GREEN << "[PARSER]: ROUTINE_DECLARATION: " << $2 << RESET << std::endl;
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $7);
    }
    | ROUTINE ID B_L PARAMETERS B_R COLON TYPE IS BODY END {
        if (shell.debug) std::cout << GREEN << "[PARSER]: ROUTINE_DECLARATION: " << $2 << RESET << std::endl;
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $9, $7);
    }
;

PARAMETERS :
    %empty {
        std::vector<ast::np<ast::VariableDeclaration>> tmp;
        $$ = tmp;
    }
    | PARAMETER_DECLARATION COMMA_SEPARATOR PARAMETERS {
        $3.push_back($1);
        $$ = $3;
    }
;

PARAMETER_DECLARATION :
    ID COLON TYPE {
        $$ = std::make_shared<ast::VariableDeclaration>($1, $3);
    }
;

BODY :
    %empty {
        std::vector<ast::np<ast::VariableDeclaration>> tmp1;
        std::vector<ast::np<ast::Statement>> tmp2;
        $$ = std::make_shared<ast::Body>(tmp1, tmp2);
    }
    | VARIABLE_DECLARATION BODY {
        $2->variables.push_back($1);
        $$ = $2;
    }
    /* | TYPE_DECLARATION BODY */
    | STATEMENT BODY {
        $2->statements.push_back($1);
        $$ = $2;
    }
;

STATEMENT :
    RETURN_STATEMENT       { $$ = $1; }
    | PRINT_STATEMENT      { $$ = $1; }
    | ASSIGNMENT_STATEMENT { $$ = $1; }
;

RETURN_STATEMENT :
    RETURN EXPRESSION SEMICOLON {
        if (shell.debug) std::cout << GREEN << "[PARSER]: RETURN_STATEMENT" << RESET << std::endl;
        $$ = std::make_shared<ast::ReturnStatement>($2);
    }
;

PRINT_STATEMENT :
    PRINT EXPRESSION SEMICOLON {
        if (shell.debug) std::cout << GREEN << "[PARSER]: PRINT_STATEMENT" << RESET << std::endl;
        $$ = std::make_shared<ast::PrintStatement>($2);
    }
;

ASSIGNMENT_STATEMENT :
    ID BECOMES EXPRESSION SEMICOLON {
        $$ = std::make_shared<ast::AssignmentStatement>(std::make_shared<ast::Identifier>($1), $3);
    }
;

%%
void cplus::parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}
