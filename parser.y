%language "C++"
%skeleton "lalr1.cc"
%require "3.2"

%define api.value.type variant
%define api.token.constructor
%define parse.assert
%define api.parser.class { parser }
%define api.namespace { cplus }

%lex-param { cplus::lexer &lexer }
%lex-param { cplus::shell &shell }
%parse-param { cplus::lexer &lexer }
%parse-param { cplus::shell &shell }

%token VAR ID IS INT_VAL REAL_VAL BOOL_VAL    // var <identifier> is \d+ \d+\.\d+ true|false
%token TYPE_KW INT_KW REAL_KW BOOLEAN_KW      // type integer real boolean
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
%type <ast::np<ast::Expression>> EXPRESSION INT_EXP
%type <ast::OperatorEnum> OPERATOR
%type <ast::np<ast::Type>> TYPE PRIMITIVE_TYPE
%type <ast::np<ast::Body>> BODY
%type <ast::np<ast::Statement>> STATEMENT
%type <ast::np<ast::ReturnStatement>> RETURN_STATEMENT
%type <ast::np<ast::PrintStatement>> PRINT_STATEMENT

%start PROGRAM

%code requires
{
    #include <iostream>
    #include <string>
    #include <vector>
    #include "ast.hpp"

    namespace cplus
    {
        class lexer;
        class shell;
    }
}

%code top
{
    #include "lexer.h"
    #include "shell.hpp"

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
        if (shell.debug) std::cout << "[PARSER]: EOF\n";
    }
    | VARIABLE_DECLARATION PROGRAM {
        if(shell.debug) std::cout << "[PARSER]: VARABLE_DECLARATION" << std::endl;
        program->variables.push_back($1);
        shell.prompt();
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
    INT_EXP {
        $$ = $1;
    }
;

OPERATOR :
    PLUS { $$ = ast::OperatorEnum::PLUS; }
;


INT_EXP:
    INT_VAL {
        $$ = std::make_shared<ast::IntegerLiteral>($1);
    }
    | B_L INT_EXP B_R           { $$ = $2; }
    | INT_EXP OPERATOR INT_EXP  { $$ = std::make_shared<ast::BinaryExpression>($1, $2, $3); }
    /* | MINUS INT_EXP             { $$ = std::make_shared<UnaryExpression>(ast::OperatorEnum::MINUS, $2); } */
;

TYPE :
    PRIMITIVE_TYPE
;

PRIMITIVE_TYPE:
    INT_KW {
        $$ = std::make_shared<ast::IntegerType>();
    }
;

ROUTINE_DECLARATION :
    ROUTINE ID B_L PARAMETERS B_R IS BODY END {
        if (shell.debug) std::cout << "[PARSER]: ROUTINE_DECLARATION: " << $2 << std::endl;
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $7);
        shell.prompt();
    }
    | ROUTINE ID B_L PARAMETERS B_R COLON TYPE IS BODY END {
        if (shell.debug) std::cout << "[PARSER]: ROUTINE_DECLARATION: " << $2 << std::endl;
        $$ = std::make_shared<ast::RoutineDeclaration>($2, $4, $9, $7);
        shell.prompt();
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
    RETURN_STATEMENT { $$ = $1; }
    | PRINT_STATEMENT { $$ = $1; }
;

RETURN_STATEMENT : RETURN EXPRESSION SEMICOLON {
    if (shell.debug) std::cout << "[PARSER]: RETURN_STATEMENT\n";
    $$ = std::make_shared<ast::ReturnStatement>($2);
}
;

PRINT_STATEMENT :
    PRINT EXPRESSION SEMICOLON {
        if (shell.debug) std::cout << "[PARSER]: PRINT_STATEMENT\n";
        $$ = std::make_shared<ast::PrintStatement>($2);
    }

%%
void cplus::parser::error(const std::string& msg) {
    std::cout << msg << '\n';
}
