#ifndef LLVM_H
#define LLVM_H

#include <llvm/IR/Module.h>
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
    void visit(ast::ArrayType *at) override;
    void visit(ast::RecordType *rt) override;
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
    void visit(ast::AssignmentStatement *stmt) override;
    void visit(ast::IfStatement *stmt) override;
    void visit(ast::WhileLoop *stmt) override;

    llvm::Value *exp_to_bool(llvm::Value *cond);

private:
    llvm::LLVMContext context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value*> location;

    llvm::Value *tmp_v = nullptr;
    llvm::Value *tmp_p = nullptr;
    llvm::Type *tmp_t = nullptr;

    llvm::IntegerType *int_t, *bool_t;
    llvm::Type *real_t;

    llvm::Constant *fmt_lld = nullptr;
    llvm::Constant *fmt_f = nullptr;
    llvm::Constant *fmt_s = nullptr;

    int spaces = 0;
    bool global_vars_pass = true;
    bool is_first_routine = true;

    llvm::Value *pop_v();
    llvm::Value *pop_p();
    llvm::Type *pop_t();
};

#endif // LLVM_H