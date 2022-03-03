#ifndef LLVM_H
#define LLVM_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "ast.hpp"
#include "shell.hpp"

// Visits AST nodes and generates LLVM IR code.
class IRGenerator : public Visitor {
public:
    IRGenerator();
    void generate();
    void visit(ast::Program *program) override;
    void visit(ast::IntType *it) override;
    void visit(ast::RealType *rt) override;
    void visit(ast::BoolType *bt) override;
    void visit(ast::IntLiteral *il) override;
    void visit(ast::RealLiteral *rl) override;
    void visit(ast::BoolLiteral *bl) override;
    void visit(ast::VariableDeclaration *vardecl) override;
    void visit(ast::Identifier *id) override;
    void visit(ast::UnaryExpression *exp) override;
    void visit(ast::BinaryExpression *exp) override;
    void visit(ast::RoutineDeclaration *routine) override;
    void visit(ast::Body *body) override;
    void visit(ast::ReturnStatement *stmt) override;
    void visit(ast::PrintStatement *stmt) override;

private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value*> named_values;

    llvm::Value* tmp_v = nullptr;
    llvm::Type* tmp_t = nullptr;

    int spaces = 0;

    llvm::Value* pop_tmp_v();
    llvm::Type* pop_tmp_t();

    std::map<ast::OperatorEnum, std::string> op_to_str = {
        { ast::OperatorEnum::PLUS, "+" },
        { ast::OperatorEnum::MINUS, "-" },
        { ast::OperatorEnum::MUL, "*" },
        { ast::OperatorEnum::DIV, "/" },
        { ast::OperatorEnum::MOD, "%" },
        { ast::OperatorEnum::AND, "%" },
        { ast::OperatorEnum::OR, "|" },
        { ast::OperatorEnum::XOR, "^" },
        { ast::OperatorEnum::NOT, "~" },
        { ast::OperatorEnum::LEQ, "<=" },
        { ast::OperatorEnum::GEQ, ">=" },
        { ast::OperatorEnum::LT, "<" },
        { ast::OperatorEnum::GT, ">" },
        { ast::OperatorEnum::EQ, "=" },
        { ast::OperatorEnum::NEQ, "/=" }
    };
};

#endif // LLVM_H