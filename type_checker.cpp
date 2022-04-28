#include "type_checker.hpp"

#define RED         "\033[31m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define RESET       "\033[0m"

#define GDEBUG(X)   if (shell.debug) std::cout << CYAN << X << RESET << std::endl;
#define GWARNING(X) std::cerr << YELLOW << "[TYPE]: [WARNINGS]:\n" << X << RESET << std::endl;
#define GERROR(X)   std::cerr << RED << "[TYPE]: [ERROR]: " << X << RESET << std::endl; std::exit(1);

#define BLOCK_B(X)                                           \
    if (shell.debug) {                                       \
        std::cout << CYAN;                                   \
        for (int i=0; i<spaces; i++) {                       \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "<" << X << ">" << RESET << std::endl;  \
        spaces += 4;                                         \
    }

#define BLOCK_E(X)                                           \
    if (shell.debug) {                                       \
        spaces -= 4;                                         \
        std::cout << CYAN;                                   \
        for (int i=0; i<spaces; i++) {                       \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "</" << X << ">" << RESET << std::endl; \
    }

extern cplus::Shell shell;

namespace ast {

TypeChecker::TypeChecker() {

}

bool TypeChecker::type_equal(node_ptr<Type> type1, node_ptr<Type> type2) {
	if (std::dynamic_pointer_cast<IntType> (type1) && std::dynamic_pointer_cast<IntType> (type2))
		return true;
	if (std::dynamic_pointer_cast<RealType> (type1) && std::dynamic_pointer_cast<RealType> (type2))
		return true;
	if (std::dynamic_pointer_cast<BoolType> (type1) && std::dynamic_pointer_cast<BoolType> (type2))
		return true;
	
	if ((std::dynamic_pointer_cast<IntType> (type1) ||
		 std::dynamic_pointer_cast<RealType> (type1) ||
		 std::dynamic_pointer_cast<BoolType> (type1))
		 &&
		 (std::dynamic_pointer_cast<IntType> (type2) ||
		 std::dynamic_pointer_cast<RealType> (type2) ||
		 std::dynamic_pointer_cast<BoolType> (type2))){
		GWARNING("Simple types casting")
		return true;
	}
		
	node_ptr<ArrayType> array1 = std::dynamic_pointer_cast<ArrayType> (type1);
	node_ptr<ArrayType> array2 = std::dynamic_pointer_cast<ArrayType> (type2);
	if (array1 && array2) {
		return type_equal(array1->dtype, array2->dtype);
	}
	
	node_ptr<RecordType> record1 = std::dynamic_pointer_cast<RecordType> (type1);
	node_ptr<RecordType> record2 = std::dynamic_pointer_cast<RecordType> (type2);
	if (record1 && record2) {
		if (record1->fields.size() != record2->fields.size())
			return false;
		for (int i = 0; i < (int)record1->fields.size(); i++) {
			record1->fields[i]->accept(this);
			node_ptr<Type> type__ = type_;
			record2->fields[i]->accept(this);
			if (!type_equal(type__, type_))
				return false;
		}
		return true;
	}
	
	node_ptr<FunctionType> function1 = std::dynamic_pointer_cast<FunctionType> (type1);
	node_ptr<FunctionType> function2 = std::dynamic_pointer_cast<FunctionType> (type2);
	if (function1 && function2) {
		return (type_equal(function1->from, function2->from) &&
				type_equal(function1->to, function2->to));
	}
	return false;
}

int TypeChecker::find_in_context_expr(std::string name) {
	for (int i = (int)context_expr.size() - 1; i >= 0; i--) {
		if (context_expr[i].first == name)
			return i;
	}
	return -1;
}

std::string TypeChecker::build_variable_name() {
	std::string res;
	for (auto& s : record_stack) {
		res += s + ".";
	}
	return res;
}

void TypeChecker::visit(ast::Program *program) {
    BLOCK_B("Program")

	for (auto u = program->variables.rbegin(); u != program->variables.rend(); u++) {
        (*u)->accept(this);
    }

    for (auto& u : program->routines) {
        u->accept(this);
    }

    context_expr.clear();

    BLOCK_E("Program")
}

void TypeChecker::visit(ast::VariableDeclaration *var) {
    BLOCK_B("VariableDeclaration")
    if (var->dtype) {
    	if (var->initial_value) {
    		var->initial_value->accept(this);
    		if (!type_equal(var->dtype, type_)) {
    			GERROR("Variable declaration type and value have different types")
    		}
    	}
    	type_ = var->dtype;
    	context_expr.push_back({build_variable_name() + var->name, var->dtype});
    }
    else {
    	if (var->initial_value) {
    		var->initial_value->accept(this);
    		context_expr.push_back({var->name, type_});
    	}
    	else {
    		GERROR("Variable declaration has neither type nor value")
    	}
    }
    
    if (node_ptr<ArrayType> array = std::dynamic_pointer_cast<ArrayType> (type_)) {
    	array->accept(this);
    }
    if (node_ptr<RecordType> record = std::dynamic_pointer_cast<RecordType> (type_)) {
    	record_stack.push_back(var->name);
    	record->accept(this);
    	record_stack.pop_back();
    }

    BLOCK_E("VariableDeclaration")
}

void TypeChecker::visit(ast::Identifier *id) {
    BLOCK_B("Identifier")
    
    int pos = find_in_context_expr(id->name);
	if (pos == -1) {
		GERROR(id->name << " is not declared")
	}
    if (id->idx) {
    	id->idx->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_)) {
			GERROR("Array index expression has not int type")
		}
		if (!std::dynamic_pointer_cast<ArrayType> (context_expr[pos].second)) {
			GERROR("Indexation applied to not array type")
		}
    }
    else {
		type_ = context_expr[pos].second;
    }
    
    BLOCK_E("Identifier")
}

void TypeChecker::visit(ast::UnaryExpression *exp) {
    BLOCK_B("UnaryExpression")
    
    exp->operand->accept(this);
    
    if (exp->op == OperatorEnum::MINUS) {
    	if (!std::dynamic_pointer_cast<IntType> (type_) &&
    		!std::dynamic_pointer_cast<RealType> (type_)) {
    		GERROR("Unary minus applied to neither int nor real")
    	}
    	if (std::dynamic_pointer_cast<IntType> (type_))
    		type_ = std::make_shared<IntType> ();
    	else
    		type_ = std::make_shared<RealType> ();
    }
    
    if (exp->op == OperatorEnum::NOT) {
    	if (!std::dynamic_pointer_cast<BoolType> (type_)) {
    		GERROR("Unary not applied to not bool")
    	}
    	type_ = std::make_shared<BoolType> ();
    }

    BLOCK_E("UnaryExpression")
}

void TypeChecker::visit(ast::BinaryExpression *exp) {
    BLOCK_B("BinaryExpression")
    
    if (exp->op == OperatorEnum::PLUS ||
    	exp->op == OperatorEnum::MINUS ||
    	exp->op == OperatorEnum::MUL ||
    	exp->op == OperatorEnum::DIV) {

    	std::string op;
    	if (exp->op == OperatorEnum::PLUS) op = "Plus";
    	if (exp->op == OperatorEnum::MINUS) op = "Minus";
    	if (exp->op == OperatorEnum::MOD) op = "Mul";
    	if (exp->op == OperatorEnum::DIV) op = "Div";
    	
    	exp->lhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_) &&
    		!std::dynamic_pointer_cast<RealType> (type_) &&
    		!std::dynamic_pointer_cast<BoolType> (type_)) {
    		GERROR(op << " left operand is neither int nor real nor bool type")
    	}
    	bool left_is_int = false;
    	if (std::dynamic_pointer_cast<IntType> (type_))
	    	left_is_int = true;

    	exp->rhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_) &&
    		!std::dynamic_pointer_cast<RealType> (type_) &&
    		!std::dynamic_pointer_cast<BoolType> (type_)) {
    		GERROR(op << " right operand is neither int nor real nor bool type")
    	}
    	if (left_is_int && std::dynamic_pointer_cast<IntType> (type_))
    		type_ = std::make_shared<IntType> ();
    	else
    		type_ = std::make_shared<RealType> ();
    }
    
    if (exp->op == OperatorEnum::MOD) {
    	exp->lhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_)) {
    		GERROR("Mod left operand is not int type")
    	}
    	exp->rhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_)) {
    		GERROR("Mod right operand is not int type")
    	}
    	type_ = std::make_shared<IntType> ();
    }
    
    if (exp->op == OperatorEnum::AND ||
    	exp->op == OperatorEnum::OR ||
    	exp->op == OperatorEnum::XOR) {

    	std::string op;
    	if (exp->op == OperatorEnum::AND) op = "And";
    	if (exp->op == OperatorEnum::OR) op = "Or";
    	if (exp->op == OperatorEnum::XOR) op = "Xor";

    	exp->lhs->accept(this);
    	if (!std::dynamic_pointer_cast<BoolType> (type_)) {
    		GERROR(op << " left operand is not bool type")
    	}
    	exp->rhs->accept(this);
    	if (!std::dynamic_pointer_cast<BoolType> (type_)) {
    		GERROR(op << " right operand is not bool type")
    	}
    	type_ = std::make_shared<BoolType> ();
    }
    
    if (exp->op == OperatorEnum::EQ ||
    	exp->op == OperatorEnum::NEQ ||
    	exp->op == OperatorEnum::GT ||
    	exp->op == OperatorEnum::LT ||
    	exp->op == OperatorEnum::GEQ ||
    	exp->op == OperatorEnum::LEQ) {

    	std::string op;
    	if (exp->op == OperatorEnum::EQ) op = "Eq";
    	if (exp->op == OperatorEnum::NEQ) op = "Neq";
    	if (exp->op == OperatorEnum::GT) op = "Gt";
    	if (exp->op == OperatorEnum::LT) op = "Lt";
    	if (exp->op == OperatorEnum::GEQ) op = "Geq";
    	if (exp->op == OperatorEnum::LEQ) op = "Leq";
    	
    	exp->lhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_) &&
    		!std::dynamic_pointer_cast<RealType> (type_)) {
    		GERROR(op << " left operand is neither int nor real type")
    	}
    	exp->rhs->accept(this);
    	if (!std::dynamic_pointer_cast<IntType> (type_) &&
    		!std::dynamic_pointer_cast<RealType> (type_)) {
    		GERROR(op << " right operand is neither int nor real type")
    	}
    	type_ = std::make_shared<BoolType> ();
    }

    BLOCK_E("BinaryExpression")
}

void TypeChecker::visit(ast::EmptyType *it) {
    GERROR("EmptyType is visited")
}

void TypeChecker::visit(ast::IntType *it) {
    GERROR("IntType is visited")
}

void TypeChecker::visit(ast::RealType *rt) {
    GERROR("RealType is visited")
}

void TypeChecker::visit(ast::BoolType *bt) {
    GERROR("BoolType is visited")
}

void TypeChecker::visit(ast::ArrayType *at) {
    BLOCK_B("ArrayType")

    at->size->accept(this);
    if (!std::dynamic_pointer_cast<IntType> (type_)) {
    	GERROR("Array size expression has not int type")
    }

    BLOCK_E("ArrayType")
}

void TypeChecker::visit(ast::RecordType *rt) {
    BLOCK_B("RecordType")

	for (auto& f : rt->fields) {
		f->accept(this);
	}

    BLOCK_E("RecordType")
}

void TypeChecker::visit(ast::FunctionType *ft) {
	GERROR("FunctionType is visited")
}

void TypeChecker::visit(ast::IntLiteral *il) {
	type_ = std::make_shared<IntType> ();
}

void TypeChecker::visit(ast::RealLiteral *rl) {
    type_ = std::make_shared<RealType> ();
}

void TypeChecker::visit(ast::BoolLiteral *bl) {
    type_ = std::make_shared<BoolType> ();
}

void TypeChecker::visit(ast::RoutineDeclaration *routine) {
    BLOCK_B("RoutineDeclaration")

	for (auto p = routine->params.rbegin(); p != routine->params.rend(); p++) {
    	(*p)->accept(this);
    }

    routine->body->accept(this);
    
    node_ptr<Type> return_type;
    if (context_return.empty()) {
    	return_type = std::make_shared<EmptyType> ();
    }
    else {
    	return_type = context_return[0];
    }

    type_ = return_type;
    
    if (routine->rtype && !type_equal(routine->rtype, type_)) {
    	GERROR("Routine specified and actual return types are different")
    }
    
    for (auto& p : routine->params) {
    	type_ = std::make_shared<FunctionType> (context_expr.back().second, type_);
    	context_expr.pop_back();
    }
    context_expr.push_back({routine->name, type_});
    
    if (context_return.size() >= 2) {
    	for (int i = 1; i < (int)context_return.size(); i++) {
    		if (!type_equal(context_return[0], context_return[i])) {
    			GERROR("Return statements have different types")
    		}
    	}
    }
    context_return.clear();

    BLOCK_E("RoutineDeclaration")
}

void TypeChecker::visit(ast::Body *body) {
    BLOCK_B("Body")
    
    size_t body_context_size = context_expr.size();

	for (auto u = body->variables.rbegin(); u != body->variables.rend(); u++) {
        (*u)->accept(this);
    }
    
	for (auto u = body->statements.rbegin(); u != body->statements.rend(); u++) {
    	(*u)->accept(this);
    }
    
    while(context_expr.size() != body_context_size) {
    	context_expr.pop_back();
    }

    BLOCK_E("Body")
}

void TypeChecker::visit(ast::ReturnStatement *stmt) {
    BLOCK_B("ReturnStatement")

	if (stmt->exp) {	
		stmt->exp->accept(this);
		context_return.push_back(type_);
	}

    BLOCK_E("ReturnStatement")
}

void TypeChecker::visit(ast::PrintStatement *stmt) {
    BLOCK_B("PrintStatement")

    if (stmt->exp) {
    	stmt->exp->accept(this);
    	if (!std::dynamic_pointer_cast <IntType> (type_) &&
    		!std::dynamic_pointer_cast <RealType> (type_) &&
    		!std::dynamic_pointer_cast <BoolType> (type_)) {
    		GERROR("Print operand is neither int nor real nor bool type")
    	}
    }

    BLOCK_E("PrintStatement")
}

void TypeChecker::visit(ast::AssignmentStatement *stmt) {
    BLOCK_B("AssignmentStatement")
    
    stmt->id->accept(this);
    node_ptr<Type> type__ = type_;
    
    stmt->exp->accept(this);
    if (!type_equal(type_, type__)) {
    	GERROR("Variable and expression in assignment have different types")
    }

    BLOCK_E("AssignmentStatement")
}

void TypeChecker::visit(ast::IfStatement *stmt) {
    BLOCK_B("IfStatement")

    stmt->cond->accept(this);
    if (!std::dynamic_pointer_cast <IntType> (type_) &&
		!std::dynamic_pointer_cast <RealType> (type_) &&
		!std::dynamic_pointer_cast <BoolType> (type_)) {
    	GERROR("If condition has neither int nor real nor bool type")
    }
    stmt->then_body->accept(this);
    if (stmt->else_body)
	    stmt->else_body->accept(this);

    BLOCK_E("IfStatement")
}

void TypeChecker::visit(ast::WhileLoop *stmt) {
    BLOCK_B("WhileLoop")
    
    stmt->cond->accept(this);
    if (!std::dynamic_pointer_cast <IntType> (type_) &&
		!std::dynamic_pointer_cast <RealType> (type_) &&
		!std::dynamic_pointer_cast <BoolType> (type_)) {
    	GERROR("While condition has neither int nor real nor bool type")
    }
    stmt->body->accept(this);

    BLOCK_E("WhileLoop")
}

// For loop is just a fancy while loop :)
void TypeChecker::visit(ast::ForLoop *stmt) {
    BLOCK_B("ForLoop")
    
    stmt->loop_var->accept(this);
    
    stmt->cond->accept(this);
    if (!std::dynamic_pointer_cast <IntType> (type_) &&
		!std::dynamic_pointer_cast <RealType> (type_) &&
		!std::dynamic_pointer_cast <BoolType> (type_)) {
    	GERROR("For condition has neither int nor real nor bool type")
    }
    stmt->action->accept(this);
    stmt->body->accept(this);
    
    context_expr.pop_back();

    BLOCK_E("ForLoop")
}

void TypeChecker::visit(ast::RoutineCall *stmt) {
    BLOCK_B("RoutineCall")

    int pos = find_in_context_expr(stmt->routine->name);
    if (pos == -1) {
    	GERROR("Routine " << stmt->routine->name << " is not declared")
    }
    
    node_ptr<Type> type__ = context_expr[pos].second;
    for (auto a = stmt->args.rbegin(); a != stmt->args.rend(); a++) {
    	node_ptr<FunctionType> function_type = std::dynamic_pointer_cast<FunctionType> (type__);
    	if (!function_type) {
    		GERROR("Application applied to not function")
    	}
    	(*a)->accept(this);
    	if (!type_equal(function_type->from, type_)) {
    		GERROR("Application types of argument mismatch")
    	}
    	type__ = function_type->to;
    }
    type_ = type__;

    BLOCK_E("RoutineCall")
}
} // namespace ast
