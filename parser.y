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
%token TYPE INT_KW REAL_KW BOOLEAN_KW         // type integer real boolean
%token B_L B_R SB_L SB_R CB_L CB_R            // ( ) [ ] { }
%token COLON SEMICOLON COMMA DOT DDOT BECOMES // : ; , . .. :=
%token PLUS MINUS MUL DIV MOD                 // + - * / %
%token AND OR XOR NOT                         // and or xor not
%token LT GT EQ LEQ GEQ NEQ                   // < > = <= >= /=
%token ARRAY RECORD ROUTINE RETURN END        // array record routine return end
%token PRINT                                  // print
%token IF THEN ELSE WHILE FOR IN LOOP REVERSE // if then else while for in loop reverse

%type <std::string> ID

// TODO: make them literals
%type <int> INT_VAL INT_EXP
%type <double> REAL_VAL REAL_EXP
%type <bool> BOOL_VAL BOOL_EXP

%type <ast::np<ast::Variable> > VariableDeclaration ModifiablePrimary ParameterDeclaration
%type <std::map<std::string, ast::np<ast::Variable> > > VariableDeclarations Parameters
%type <ast::np<ast::ExpressionNode> > Expression
%type <std::vector<ast::np<ast::ExpressionNode>>> Expressions
%type <ast::np<ast::TypeNode> > Type PrimitiveType UserType ArrayType RecordType TypeDeclaration
%type <ast::np<ast::Literal> > ModifiablePrimaryEq


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
    #include "sa.hpp"

    static cplus::parser::symbol_type yylex(cplus::lexer &lexer, cplus::shell &shell) {
        return lexer.get_next_token();
    }
    
    using namespace cplus;
    using namespace ast;
    
    np<Program> program = make_shared<Program>();  // Points to the whole program node.
    sa semantic_analyzer;
}


%%

// Used for constructing semicolon separated items
SemicolonSeparator : SEMICOLON | %empty
;

// Used for constructing comma separated items
CommaSeparator : COMMA | %empty
;

Program:
    %empty { if (shell.pdebug) cout << "[PARSER]: EOF\n"; }
    | SimpleDeclaration SemicolonSeparator Program
    | RoutineDeclaration SemicolonSeparator Program
    | Statement SemicolonSeparator Program
;

SimpleDeclaration:
    VariableDeclaration {
        if(shell.pdebug) cout << "[PARSER]: " << *$1 << '\n';
        program->variables[$1->name] = $1;
        shell.prompt();
    }
    | TypeDeclaration {
        shell.prompt();
    }
;

VariableDeclaration:
    VAR ID IS Expression SEMICOLON {
        auto exp = $4;
        $$ = make_shared<Variable> (exp->dtype, $2, exp->value);
    }

    | VAR ID COLON Type SEMICOLON {
        $$ = make_shared<Variable> ($4, $2, make_shared<Literal>());
    }

    | VAR ID COLON Type IS Expression SEMICOLON {
        auto exp = $6;
        $$ = make_shared<Variable> (exp->dtype, $2, exp->value);
    }
;

// TODO: Why do we need ExpressionNode again? It seems to do the same job as Literal
// TODO: Expression can contain a RoutineCall
// Also, why literal constructor need the dtype
Expression :
    INT_EXP {
        auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::INTEGER));
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    | REAL_EXP {
        auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::REAL));
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    | BOOL_EXP {
        auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    | ModifiablePrimaryEq {
        $$ = make_shared<ExpressionNode>($1->dtype, $1);
    }
;

// Simplifies to a Literal
ModifiablePrimaryEq :
    ModifiablePrimary {
        auto var = $1;
        $$ = var->value;
    }
    | ModifiablePrimary PLUS ModifiablePrimaryEq {
        auto var = $1;
        $$ = var->value->add($3);
    }
    
    // TODO: Add more rules for MINUS, MUL, DIV, MOD, EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, XOR, NOT 

// TODO: implement the same functionality in a shorter/smarter way
INT_EXP: INT_VAL              { $$ = $1; }
    | B_L INT_EXP B_R         { $$ = $2; }
    | INT_EXP PLUS INT_EXP    { $$ = $1 + $3; }
    | INT_EXP MINUS INT_EXP   { $$ = $1 - $3; }
    | INT_EXP MUL INT_EXP     { $$ = $1 * $3; }
    | INT_EXP MOD INT_EXP     { $$ = $1 % $3; }
;

REAL_EXP: REAL_VAL               { $$ = $1; }
    | B_L REAL_EXP B_R           { $$ = $2; }
    
    // real real
    | REAL_EXP PLUS REAL_EXP     { $$ = $1 + $3; }
    | REAL_EXP MINUS REAL_EXP    { $$ = $1 - $3; }
    | REAL_EXP MUL REAL_EXP      { $$ = $1 * $3; }
    | REAL_EXP DIV REAL_EXP      { $$ = $1 / $3; }

    // int real
    | INT_EXP PLUS REAL_EXP      { $$ = $1 + $3; }
    | INT_EXP MINUS REAL_EXP     { $$ = $1 - $3; }
    | INT_EXP MUL REAL_EXP       { $$ = $1 * $3; }
    | INT_EXP DIV REAL_EXP       { $$ = $1 / $3; }

    // real int
    | REAL_EXP PLUS INT_EXP      { $$ = $1 + $3; }
    | REAL_EXP MINUS INT_EXP     { $$ = $1 - $3; }
    | REAL_EXP MUL INT_EXP       { $$ = $1 * $3; }
    | REAL_EXP DIV INT_EXP       { $$ = $1 / $3; }

    // int int
    | INT_EXP DIV INT_EXP        { $$ = $1 / (double)$3; }
;

BOOL_EXP : BOOL_VAL          { $$ = $1; }
    | B_L BOOL_EXP B_R       { $$ = $2; }
    
    // bool bool
    | BOOL_EXP AND BOOL_EXP  { $$ = $1 && $3; }
    | BOOL_EXP OR BOOL_EXP   { $$ = $1 || $3; }
    | BOOL_EXP XOR BOOL_EXP  { $$ = $1 != $3; }
    | NOT BOOL_EXP           { $$ = !($2); }

    // int int
    | INT_EXP LT INT_EXP     { $$ = $1 < $3; }
    | INT_EXP LEQ INT_EXP    { $$ = $1 <= $3; }
    | INT_EXP GT INT_EXP     { $$ = $1 > $3; }
    | INT_EXP GEQ INT_EXP    { $$ = $1 >= $3; }
    | INT_EXP EQ INT_EXP     { $$ = ($1 == $3); }
    | INT_EXP NEQ INT_EXP    { $$ = ($1 != $3); }

    // real real
    | REAL_EXP LT REAL_EXP   { $$ = $1 < $3; }
    | REAL_EXP LEQ REAL_EXP  { $$ = $1 <= $3; }
    | REAL_EXP GT REAL_EXP   { $$ = $1 > $3; }
    | REAL_EXP GEQ REAL_EXP  { $$ = $1 >= $3; }
    | REAL_EXP EQ REAL_EXP   { $$ = ($1 == $3); }
    | REAL_EXP NEQ REAL_EXP  { $$ = ($1 != $3); }

    // real integer
    | REAL_EXP LT INT_EXP    { $$ = $1 < $3; }
    | REAL_EXP LEQ INT_EXP   { $$ = $1 <= $3; }
    | REAL_EXP GT INT_EXP    { $$ = $1 > $3; }
    | REAL_EXP GEQ INT_EXP   { $$ = $1 >= $3; }
    | REAL_EXP EQ INT_EXP    { $$ = ($1 == $3); }
    | REAL_EXP NEQ INT_EXP   { $$ = ($1 != $3); }

    // integer real
    | INT_EXP LT REAL_EXP    { $$ = $1 < $3; }
    | INT_EXP LEQ REAL_EXP   { $$ = $1 <= $3; }
    | INT_EXP GT REAL_EXP    { $$ = $1 > $3; }
    | INT_EXP GEQ REAL_EXP   { $$ = $1 >= $3; }
    | INT_EXP EQ REAL_EXP    { $$ = ($1 == $3); }
    | INT_EXP NEQ REAL_EXP   { $$ = ($1 != $3); }
;


TypeDeclaration : TYPE ID IS Type SEMICOLON {
    if (shell.pdebug) {
        cout << "[PARSER]: alias " << $2 << " for type " << *$4 << '\n';
    }
    program->types[$2] = $4;
}
;

Type : PrimitiveType | UserType | ID {
    $$ = program->types[$1];
}
;

PrimitiveType :
    INT_KW {
        $$ = make_shared<TypeNode>(make_shared<Primitive>(ast::INTEGER));
    }
    | REAL_KW {
        $$ = make_shared<TypeNode>(make_shared<Primitive>(ast::REAL));
    }
    | BOOLEAN_KW {
        $$ = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
    }
;

UserType : ArrayType | RecordType
;

ArrayType : ARRAY SB_L INT_EXP SB_R Type {
    $$ = make_shared<TypeNode>(make_shared<Array>($3, $5));
}
;

RecordType : RECORD CB_L VariableDeclarations CB_R END {
    $$ = make_shared<TypeNode>(make_shared<Record>($3));
}
;

VariableDeclarations:
    %empty {
        map< string, np<Variable> > tmp;
        $$ = tmp;
    }
    | VariableDeclaration SemicolonSeparator VariableDeclarations {
        auto vars = $3;
        auto var = $1;
        vars[var->name] = var;
        $$ = vars;
    }
;

// TODO: detecting rtype in the first case?
RoutineDeclaration :
    ROUTINE ID B_L Parameters B_R IS Body END {
        if (shell.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
        program->routines[$2] = make_shared<Routine>($2, $4, make_shared<TypeNode>());
        shell.prompt();
    }
    | ROUTINE ID B_L Parameters B_R COLON Type IS Body END {
        if (shell.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
        program->routines[$2] = make_shared<Routine>($2, $4, $7);
        shell.prompt();
    }
;

Parameters :
    %empty {
        map< string, np<Variable> > tmp;
        $$ = tmp;
    }
    | ParameterDeclaration CommaSeparator Parameters {
        auto vars = $3;
        auto var = $1;
        vars[var->name] = var;
        $$ = vars;
    }
;

ParameterDeclaration :
    ID COLON Type {
        $$ = make_shared<Variable>($3, $1, make_shared<Literal>());
    }
;

Body : %empty
    | SimpleDeclaration SemicolonSeparator Body
    | Statement SemicolonSeparator Body
;

Statement : Assignment | RoutineCall | WhileLoop | ForLoop | IfStatement | ReturnStatement | PrintStatement
;

Assignment : ModifiablePrimary BECOMES Expression SEMICOLON {
    if (shell.pdebug) cout << "[PARSER]: Assignment statement parsed\n";
    auto var = $1;
    auto exp = $3;
    var->value = exp->value;
    shell.prompt();
}
;

// Represents a variable name, an array element, or a record field
ModifiablePrimary :
    ID DOT ID {
        if(!semantic_analyzer.checkRecordAccess($1, $3)){
            error("Semantic error");
            return 1;
        }
        $$ = get<np<Record>>(program->types[$1]->dtype)->variables[$3];
    }

    | ID SB_L INT_EXP SB_R {
        if(!semantic_analyzer.checkArrayAccess($1, $3)){
            error("Semantic error");
            return 1;
        }
        $$ = get<np<Array>>(program->types[$1]->dtype)->data[$3-1];
    }

    | ID {
        if(!semantic_analyzer.checkVariableAccess($1)){
            error("Semantic error");
            return 1;
        }
        $$ = program->variables[$1];
    }

RoutineCall : ID B_L Expressions B_R SEMICOLON {
    if (shell.pdebug) cout << "[PARSER]: routine call for " << $1 << " parsed\n";
    auto exps = $3;
    if(!semantic_analyzer.checkRoutineCall($1, $3.size())){
        error("Semantic error");
        return 1;
    }
}
;

Expressions :
    %empty {
        vector<np<ExpressionNode> > tmp;
        $$ = tmp;
    }
    | Expression CommaSeparator Expressions {
        auto exp = $1;
        auto exps = $3;
        exps.push_back(exp);
        $$ = exps;
    }
;

IfStatement :
    IF Expression THEN Body END {
        if (shell.pdebug) cout << "[PARSER]: if statement parsed\n";
    }
    | IF Expression THEN Body ELSE Body END {
        if (shell.pdebug) cout << "[PARSER]: if-else statement parsed\n";
    }
;

ReturnStatement :
    RETURN Expression SEMICOLON {
        if (shell.pdebug) cout << "[PARSER]: return statement parsed\n";
    }
;

// TODO: Generalize to PRINT Expression
PrintStatement :
    PRINT ModifiablePrimary SEMICOLON {
        if (shell.pdebug) cout << "[PARSER]: print " << *$2 << "\n";
        cout << *($2->value) << '\n';
        shell.prompt();
    }
    | PRINT RoutineCall SEMICOLON
;


WhileLoop : WHILE Expression LOOP Body END {
    if (shell.pdebug) cout << "[PARSER]: while loop parsed\n";
}
;

ForLoop :
    FOR ID IN Range LOOP Body END {
        if (shell.pdebug) cout << "[PARSER]: for loop parsed\n";
        
    }
    | FOR ID IN REVERSE Range LOOP Body END {
        if (shell.pdebug) cout << "[PARSER]: reverse for loop parsed\n";
    }
;

Range :
    Expression DDOT Expression
;

%%
void cplus::parser::error(const std::string& msg)
{
    std::cout << msg << '\n';
}
