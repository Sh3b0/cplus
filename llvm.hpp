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
    void visit(ast::IntegerType *it) override;
    void visit(ast::IntegerLiteral *il) override;
    void visit(ast::VariableDeclaration *vardecl) override;
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

    llvm::Value* pop_tmp_v();
    llvm::Type* pop_tmp_t();
};

#endif // LLVM_H