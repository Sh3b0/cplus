#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "ast.hpp"
#include "shell.hpp"

namespace ast {

// Visits AST nodes and checks types.
class TypeChecker : public Visitor {
public:
    TypeChecker();
    void generate();
    void visit(ast::Program *program) override;
    void visit(ast::EmptyType *it) override;
    void visit(ast::IntType *it) override;
    void visit(ast::RealType *rt) override;
    void visit(ast::BoolType *bt) override;
    void visit(ast::ArrayType *at) override;
    void visit(ast::RecordType *rt) override;
    void visit(ast::FunctionType *ft) override;
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
    void visit(ast::ForLoop *stmt) override;
    void visit(ast::RoutineCall *stmt) override;

private:
	node_ptr<Type> type_;
	std::vector<std::pair<std::string, node_ptr<Type>>> context_expr;
	std::vector<node_ptr<Type>> context_return;
	std::vector<std::string> record_stack;

	bool type_equal(node_ptr<Type> type1, node_ptr<Type> type2);
	int find_in_context_expr(std::string name);
	std::string build_variable_name();
	
    int spaces = 0;
};
} // namespace ast

#endif // TYPE_CHECKER_H
