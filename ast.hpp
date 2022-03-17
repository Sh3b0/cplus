#ifndef AST_H
#define AST_H

#include <iostream>
#include <memory>
#include <map>
#include <vector>

// Forward declarations
namespace ast {
struct Node;
struct Program;
struct Type;
struct Expression;
struct UnaryExpression;
struct BinaryExpression;
struct Identifier;
struct IntType;
struct RealType;
struct BoolType;
struct ArrayType;
struct RecordType;
struct IntLiteral;
struct RealLiteral;
struct BoolLiteral;
struct VariableDeclaration;
struct RoutineDeclaration;
struct Body;
struct Statement;
struct ReturnStatement;
struct PrintStatement;
struct AssignmentStatement;
struct IfStatement;
struct WhileLoop;
struct ForLoop;
struct RoutineCall;
} // namespace ast

// Base class for code generator and anything that traverses AST.
class Visitor {
public:
    virtual void visit(ast::Program *program) = 0;
    virtual void visit(ast::IntType *it) = 0;
    virtual void visit(ast::RealType *it) = 0;
    virtual void visit(ast::BoolType *it) = 0;
    virtual void visit(ast::ArrayType *at) = 0;
    virtual void visit(ast::RecordType *rt) = 0;
    virtual void visit(ast::IntLiteral *il) = 0;
    virtual void visit(ast::RealLiteral *rl) = 0;
    virtual void visit(ast::BoolLiteral *bl) = 0;
    virtual void visit(ast::VariableDeclaration *vardecl) = 0;
    virtual void visit(ast::Identifier *id) = 0;
    virtual void visit(ast::UnaryExpression *exp) = 0;
    virtual void visit(ast::BinaryExpression *exp) = 0;
    virtual void visit(ast::RoutineDeclaration *routine) = 0;
    virtual void visit(ast::Body *body) = 0;
    virtual void visit(ast::ReturnStatement *stmt) = 0;
    virtual void visit(ast::PrintStatement *stmt) = 0;
    virtual void visit(ast::AssignmentStatement *stmt) = 0;
    virtual void visit(ast::IfStatement *stmt) = 0;
    virtual void visit(ast::WhileLoop *stmt) = 0;
    virtual void visit(ast::ForLoop *stmt) = 0;
    virtual void visit(ast::RoutineCall *stmt) = 0;
};

namespace ast {

// Pointer to an AST node.
template <typename Node> using node_ptr = std::shared_ptr<Node>;

// Enumerations
enum class TypeEnum { INT, REAL, BOOL, ARRAY, RECORD };
enum class OperatorEnum { PLUS, MINUS, MUL, DIV, MOD, AND, OR, NOT, XOR, EQ, NEQ, LT, GT, LEQ, GEQ }; 

// Base class for AST nodes
struct Node {
    virtual void accept(Visitor *v) = 0;
};

// A special node containing program variables, type aliases, and routines.
struct Program : Node {
    std::vector<node_ptr<VariableDeclaration>> variables;
    std::map<std::string, node_ptr<Type>> types;
    std::vector<node_ptr<RoutineDeclaration>> routines;
    
    void accept(Visitor *v) override { v->visit(this); }
};

// Base class for Expressions
struct Expression : Node {
    node_ptr<Type> dtype;
};

// Base class for Types
struct Type : Node {
    virtual TypeEnum getType() { return TypeEnum::INT; };
    virtual void accept(Visitor *v) = 0;
};

// Base class for Statements
struct Statement : virtual Node {
    void accept(Visitor *v) override = 0;
};

// <Types>
struct IntType : Type {
    IntType() {}
    TypeEnum getType() { return TypeEnum::INT; }

    void accept(Visitor *v) override { v->visit(this); }
};

struct RealType : Type {
    RealType() {}
    TypeEnum getType() { return TypeEnum::REAL; }

    void accept(Visitor *v) override { v->visit(this); }
};

struct BoolType : Type {
    BoolType() {}
    TypeEnum getType() { return TypeEnum::BOOL; }

    void accept(Visitor *v) override { v->visit(this); }
};

struct ArrayType : Type {
    node_ptr<Expression> size;
    node_ptr<Type> dtype;
    
    ArrayType(node_ptr<Expression> size, node_ptr<Type>dtype) {
        this->size = size;
        this->dtype = dtype;
    }

    TypeEnum getType() { return TypeEnum::ARRAY; }

    void accept(Visitor *v) override { v->visit(this); }
};

struct RecordType : Type {
    std::string name; // set by llvm vardecl or typedecl
    std::vector<node_ptr<VariableDeclaration>> fields;
    
    RecordType(std::vector<node_ptr<VariableDeclaration>> fields) {
        this->fields = fields;
    }

    TypeEnum getType() { return TypeEnum::RECORD; }

    void accept(Visitor *v) override { v->visit(this); }
};

// </Types>
// <Expressions>
struct UnaryExpression : Expression {
    node_ptr<Expression> operand;
    OperatorEnum op;

    UnaryExpression(OperatorEnum op, node_ptr<Expression> operand) {
        this->operand = operand;
        this->op = op;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct BinaryExpression : Expression {
    node_ptr<Expression> lhs, rhs;
    OperatorEnum op;

    BinaryExpression(node_ptr<Expression> lhs, OperatorEnum op, node_ptr<Expression> rhs) {
        this->lhs = lhs;
        this->rhs = rhs;
        this->op = op;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct IntLiteral : Expression {
    int64_t value;

    IntLiteral(int64_t value) {
        this->dtype = std::make_shared<IntType>();
        this->value = value;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct RealLiteral : Expression {
    double value;

    RealLiteral(double value) {
        this->dtype = std::make_shared<RealType>();
        this->value = value;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct BoolLiteral : Expression {
    bool value;

    BoolLiteral(bool value) {
        this->dtype = std::make_shared<BoolType>();
        this->value = value;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct Identifier : Expression {
    std::string name;
    node_ptr<Expression> idx;
    
    // variable or record field access
    Identifier(std::string name) {
        this->name = name;
    }
    
    // array element access
    Identifier(std::string name, node_ptr<Expression> idx) {
        this->name = name;
        this->idx = idx;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

// </Expressions>
// <Nodes>
struct VariableDeclaration : Node {
    std::string name;
    node_ptr<Type> dtype;
    node_ptr<Expression> initial_value;

    VariableDeclaration(std::string name, node_ptr<Type> dtype) {
        this->name = name;
        this->dtype = dtype;
        this->initial_value = nullptr;
    }

    VariableDeclaration(std::string name, node_ptr<Expression> initial_value) {
        this->name = name;
        this->dtype = initial_value->dtype;
        this->initial_value = initial_value;
    }

    VariableDeclaration(std::string name, node_ptr<Type> dtype, node_ptr<Expression> initial_value) {
        this->name = name;
        this->dtype = dtype;
        this->initial_value = initial_value;
    }

    void accept(Visitor *v) { v->visit(this); }
};

struct Body : Node {
    std::vector<node_ptr<Statement>> statements;
    std::vector<node_ptr<VariableDeclaration>> variables;
    // std::vector<node_ptr<TypeDeclaration>> types;
    
    Body(std::vector<node_ptr<VariableDeclaration>> variables, std::vector<node_ptr<Statement>> statements) {
        this->variables = variables;
        this->statements = statements;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct RoutineDeclaration : Node {
    std::string name;
    std::vector<node_ptr<VariableDeclaration>> params;
    node_ptr<Type> rtype;
    node_ptr<Body> body;
    
    RoutineDeclaration(std::string name, std::vector<node_ptr<VariableDeclaration>> params, node_ptr<Body> body, node_ptr<Type> rtype) {
        this->name = name;
        this->params = params;
        this->rtype = rtype;
        this->body = body;
    }
    
    RoutineDeclaration(std::string name, std::vector<node_ptr<VariableDeclaration>> params, node_ptr<Body> body) {
        this->name = name;
        this->params = params;
        this->body = body;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

// </Nodes>
// <Statements>
struct ReturnStatement : Statement {
    node_ptr<Expression> exp;
    
    ReturnStatement() {}

    ReturnStatement(node_ptr<Expression> exp) {
        this->exp = exp;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct PrintStatement : Statement {
    node_ptr<Expression> exp;
    node_ptr<std::string> str;
    bool endl;

    PrintStatement(node_ptr<Expression> exp, bool endl=false) {
        this->exp = exp;
        this->endl = endl;
    }

    PrintStatement(node_ptr<std::string> str, bool endl=false) {
        this->str = str;
        this->endl = endl;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct AssignmentStatement : Statement {
    node_ptr<Identifier> id;
    node_ptr<Expression> exp;

    AssignmentStatement(node_ptr<Identifier> id, node_ptr<Expression> exp) {
        this->id = id;
        this->exp = exp;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct IfStatement : Statement {
    node_ptr<Expression> cond;
    node_ptr<Body> then_body, else_body;

    IfStatement(node_ptr<Expression> cond, node_ptr<Body> then_body) {
        this->cond = cond;
        this->then_body = then_body;
    }

    IfStatement(node_ptr<Expression> cond, node_ptr<Body> then_body, node_ptr<Body> else_body) {
        this->cond = cond;
        this->then_body = then_body;
        this->else_body = else_body;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct WhileLoop : Statement {
    node_ptr<Expression> cond;
    node_ptr<Body> body;

    WhileLoop(node_ptr<Expression> cond, node_ptr<Body> body) {
        this->cond = cond;
        this->body = body;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct ForLoop : Statement {
    node_ptr<VariableDeclaration> loop_var;
    node_ptr<Expression> cond;
    node_ptr<Body> body;
    node_ptr<AssignmentStatement> action;

    ForLoop(node_ptr<VariableDeclaration> loop_var, node_ptr<Expression> cond, node_ptr<Body> body, node_ptr<AssignmentStatement> action) {
        this->loop_var = loop_var;
        this->cond = cond;
        this->body = body;
        this->action = action;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

struct RoutineCall : Statement, Expression {
    node_ptr<RoutineDeclaration> routine;
    std::vector<node_ptr<Expression>> args;

    RoutineCall(node_ptr<RoutineDeclaration> routine, std::vector<node_ptr<Expression>> args) {
        this->routine = routine;
        this->args = args;
    }

    void accept(Visitor *v) override { v->visit(this); }
};

// </Statements>
} // namespace ast

#endif // AST_H