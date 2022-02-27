#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <variant>
#include <sstream>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>

// Forward declarations
namespace ast {
struct Node;
struct Program;
struct Type;
struct Expression;
struct BinaryExpression;
struct IntegerType;
struct IntegerLiteral;
struct VariableDeclaration;
struct RoutineDeclaration;
struct Body;
struct Statement;
struct ReturnStatement;
struct PrintStatement;
} // namespace ast

// Base class for code generator and anything that traverses AST.
class Visitor {
public:
    virtual void visit(ast::Program *program) = 0;
    virtual void visit(ast::IntegerType *it) = 0;
    virtual void visit(ast::IntegerLiteral *il) = 0;
    virtual void visit(ast::VariableDeclaration *vardecl) = 0;
    virtual void visit(ast::BinaryExpression *exp) = 0;
    virtual void visit(ast::RoutineDeclaration *routine) = 0;
    virtual void visit(ast::Body *body) = 0;
    virtual void visit(ast::ReturnStatement *stmt) = 0;
    virtual void visit(ast::PrintStatement *stmt) = 0;
};

namespace ast {

// Pointer to an AST node.
template <typename T> using np = std::shared_ptr<T>;

// Enumerations
enum class TypeEnum { INT }; // TODO
enum class OperatorEnum { PLUS }; // TODO

// Base class for AST nodes
struct Node {
    virtual void accept(Visitor *v) = 0; 
};

// A special node containing program variables, type aliases, and routines.
struct Program : Node {
    std::vector<np<VariableDeclaration>> variables;
    std::vector<np<RoutineDeclaration>> routines;
    
    void accept(Visitor *v) override { return v->visit(this); }
};

// Base class for Expressions
struct Expression : Node {
    np<Type> dtype;
};

// Base class for Types
struct Type : Node {
    virtual TypeEnum getType() { return TypeEnum::INT; };
    virtual void accept(Visitor* v) = 0;
};

struct IntegerType : Type {
    IntegerType() {};
    TypeEnum getType() { return TypeEnum::INT; }

    void accept(Visitor* v) override { v->visit(this); }
};

struct IntegerLiteral : Expression {
    uint64_t value;
    IntegerLiteral(long long value) {
        this->dtype = std::make_shared<IntegerType>();
        this->value = value;
    }

    void accept(Visitor* v) override { v->visit(this); }
};

struct VariableDeclaration : Node {
    std::string name;
    np<Type> dtype;
    np<Expression> iv;

    VariableDeclaration(std::string name, np<Type> dtype) {
        this->name = name;
        this->dtype = dtype;
        this->iv = nullptr;
    }

    VariableDeclaration(std::string name, np<Expression> iv) {
        this->name = name;
        this->dtype = iv->dtype;
        this->iv = iv;
    }

    VariableDeclaration(std::string name, np<Type> dtype, np<Expression> iv) {
        this->name = name;
        this->dtype = dtype;
        this->iv = iv;
    }

    void accept(Visitor *v) { return v->visit(this); }
};

struct BinaryExpression : Expression {
    np<Expression> lhs, rhs;
    OperatorEnum op;

    BinaryExpression(np<Expression> lhs, OperatorEnum op, np<Expression> rhs) {
        this->lhs = lhs;
        this->rhs = rhs;
        this->op = op;
    }

    void accept(Visitor *v) { return v->visit(this); }
};

struct Body : Node {
    std::vector<np<Statement>> statements;
    std::vector<np<VariableDeclaration>> variables;
    // std::vector<np<TypeDeclaration>> types;
    
    Body(std::vector<np<VariableDeclaration>> variables, std::vector<np<Statement>> statements) {
        this->variables = variables;
        this->statements = statements;
    }

    void accept(Visitor* v) override { v->visit(this); }
};

struct RoutineDeclaration : Node {
    std::string name;
    std::vector<np<VariableDeclaration>> params;
    np<Type> rtype;
    np<Body> body;
    
    RoutineDeclaration(std::string name, std::vector<np<VariableDeclaration>> params, np<Body> body, np<Type> rtype) {
        this->name = name;
        this->params = params;
        this->rtype = rtype;
        this->body = body;
    }
    
    RoutineDeclaration(std::string name, std::vector<np<VariableDeclaration>> params, np<Body> body) {
        this->name = name;
        this->params = params;
        // this->rtype = rtype; // TODO: detect rtype dynamically from return statement.
        this->body = body;
    }

    void accept(Visitor* v) override { v->visit(this); }
};

struct Statement : virtual Node {
    void accept(Visitor* v) override = 0;
};

struct ReturnStatement : Statement {
    np<Expression> exp;

    ReturnStatement(np<Expression> exp) {
        this->exp = exp;
    }

    void accept(Visitor* v) override { v->visit(this); }
};

struct PrintStatement : Statement {
    np<Expression> exp;

    PrintStatement(np<Expression> exp) {
        this->exp = exp;
    }

    void accept(Visitor* v) override { v->visit(this); }
};

} // namespace ast

#endif // AST_H