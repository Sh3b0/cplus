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

%type <ast::np<ast::Variable> > VariableDeclaration ModifiablePrimary
%type <std::map<std::string, ast::np<ast::Variable> > > VariableDeclarations
%type <ast::np<ast::ExpressionNode> > Expression
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
    static cplus::parser::symbol_type yylex(cplus::lexer &lexer, cplus::shell &shell) {
        return lexer.get_next_token();
    }
    
    using namespace cplus;
    using namespace ast;
    
    np<Program> program = make_shared<Program>();  // Points to the whole program node.
}


%%

// Used for constructing semicolon separated items
SemicolonSeparator : SEMICOLON | %empty
;

// Used for constructing comma separated items
CommaSeparator : COMMA | %empty
;

Program:
    // %empty { if (shell.pdebug) cout << "[PARSER]: EOF\n"; }
    /* | SimpleDeclaration SemicolonSeparator Program */
    RoutineDeclaration SemicolonSeparator Program
    | Body
;

SimpleDeclaration:
    VariableDeclaration {
        auto var = $1;
        program->variables[var->name] = var;
        shell.prompt();
    }
    | TypeDeclaration {
        shell.prompt();
    }
;

VariableDeclaration:
    VAR ID IS Expression SEMICOLON {
        auto exp = $4;
        
        if (shell.pdebug)
            cout << "[PARSER]: " << exp->dtype << " " << $2 << " = " << exp->value << "\n";
        
        $$ = make_shared<Variable> (exp->dtype, $2, exp->value);
    }

    | VAR ID COLON Type SEMICOLON {
        if (shell.pdebug)
            cout << "[PARSER]: " << $4 << " " << $2 << "\n";
    
        $$ = make_shared<Variable> ($4, $2, make_shared<Literal>());
    }

    | VAR ID COLON Type IS Expression SEMICOLON {
        auto exp = $6;

        if (shell.pdebug)
            cout << "[PARSER]: " << exp->dtype << " " << $2 << " = " << exp->value << "\n";
        
        $$ = make_shared<Variable> (exp->dtype, $2, exp->value);
    }
;

// TODO: Why do we need ExpressionNode again? It seems to do the same job as Literal
// Also, why literal constructor need the dtype
// TODO: Expression shouldn't be %empty
Expression :
    INT_EXP {
        if (shell.pdebug) cout << "[PARSER]: INT_EXP evaluates to " << $1 << "\n";
        auto type = make_shared<TypeNode>("integer");
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    | REAL_EXP {
        if (shell.pdebug) cout << "[PARSER]: REAL_EXP evaluates to " << $1 << "\n";
        auto type = make_shared<TypeNode>("real");
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    | BOOL_EXP {
        if (shell.pdebug) cout << "[PARSER]: BOOL_EXP evaluates to " << $1 << "\n";
        auto type = make_shared<TypeNode>("boolean");
        $$ = make_shared<ExpressionNode>(type, make_shared<Literal>(type, $1));
    }
    // TODO: Expression can also be a ModifiablePrimaryEq (e.g., a[0] + rec.item + (4 * 4) + x).
    | ModifiablePrimaryEq {
        if (shell.pdebug) cout << "[PARSER]: ModifiablePrimaryEq Parsed\n";
        auto lit = $1;
        $$ = make_shared<ExpressionNode>(lit->dtype, lit);
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
        // TODO: SemanticAnalyzer may face his worst enemy: NullPointerException
        $$ = var->value->add($3);
    }
    // TODO: Add more rules for MINUS, MUL, DIV, MOD, EQ, NEQ, LT, GT, LEQ, GEQ, AND, OR, XOR, NOT 

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
    if (shell.pdebug) cout << "[PARSER]: alias " << $2 << " for type " << $4 << "\n";
    program->types[$2] = $4;
}
;

Type : PrimitiveType | UserType | ID {
    $$ = program->types[$1];
}
;

PrimitiveType : INT_KW { $$ = make_shared<TypeNode>("integer"); }
                | REAL_KW { make_shared<TypeNode>("real"); }
                | BOOLEAN_KW { make_shared<TypeNode>("boolean"); }
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

RoutineDeclaration :
    ROUTINE ID B_L Parameters B_R IS Body END {
        if (shell.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
        program->routines.push_back(make_shared<Routine>($2));
        shell.prompt();
    }
    | ROUTINE ID B_L Parameters B_R COLON Type IS Body END {
        if (shell.pdebug) cout << "[PARSER]: routine " << $2 << " is declared\n";
        program->routines.push_back(make_shared<Routine>($2));
        shell.prompt();
    }
;

Parameters :
    %empty | ParameterDeclaration CommaSeparator Parameters
;

ParameterDeclaration :
    ID COLON Type
;

Body : %empty { if (shell.pdebug) cout << "[PARSER]: EOF\n"; }
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
        if (shell.pdebug) cout << "[PARSER]: access " << $3 << " from " << $1 << '\n';
        // TODO: SemanticAnalyezer possible errors
        // - $1 is not defined
        // - $1 is not a record
        // - $3 is not an element of $1
        $$ = get<np<Record>>(program->types[$1]->dtype)->variables[$3];
    }

    | ID SB_L INT_EXP SB_R {
        if (shell.pdebug) cout << "[PARSER]: " << $1 << "[" << $3 << "]" << "\n";
        // TODO: return the node for the variable named $1[$3]
    }

    | ID {
        if (shell.pdebug) cout << "[PARSER]: " << $1 << "\n";
        // TODO: SemanticAnalyezer possible errors: $1 is not defined.
        $$ = program->variables[$1];
    }

RoutineCall : ID B_L Expressions B_R SEMICOLON {
    if (shell.pdebug) cout << "[PARSER]: routine call for " << $1 << " parsed\n";
}
;

Expressions :
    %empty | Expression CommaSeparator Expressions
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

IfStatement :
    IF Expression THEN Body END {
        if (shell.pdebug) cout << "[PARSER]: if statement parsed\n";
    }
    | IF Expression THEN Body ELSE Body END {
        if (shell.pdebug) cout << "[PARSER]: if-else statement parsed\n";
    }
;

ReturnStatement :
    RETURN ID SEMICOLON {
        if (shell.pdebug) cout << "[PARSER]: return statement parsed\n";
    }
;

// Print statements are allowed only inside routines.
// For now, if a print statement was encountered in a routine declaration,
// it will be executed immediately, this can help with debugging for now.

// Future behaviour: program starts from "main" routine.
// Other routines should only be stored and executed only upon call. 

// TODO: Generalize to PRINT Expression
PrintStatement :
    PRINT ModifiablePrimary SEMICOLON {
        if (shell.pdebug) cout << "[PARSER]: print " << $2 << "\n";
        cout << *($2->value) << '\n';
        shell.prompt();
    }
    | PRINT RoutineCall SEMICOLON // TODO
;

%%
void cplus::parser::error(const std::string& msg)
{
    std::cout << msg << '\n';
}
