#include "llvm.hpp"

extern cplus::shell shell;

// Constructor
IRGenerator::IRGenerator() : builder(context) {
    module = std::make_unique<llvm::Module>(llvm::StringRef("ir.ll"), context);
}

// Emits IR code as "ir.ll"
void IRGenerator::generate() {
    if (shell.debug) std::cout << "[LLVM]: <IRGenerator>" << std::endl;

    auto CPU = "generic";
    auto Features = "";

    module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    std::string msg;
    llvm::raw_string_ostream out(msg);
    if(llvm::verifyModule(*this->module, &out)) {
        llvm::errs() << "[LLVM]: Warning: " << out.str();
    }

    FILE* f = fopen("ir.ll", "w");
    llvm::raw_fd_ostream outfile(fileno(f), true);
    module->print(outfile, nullptr);

    if (shell.debug) std::cout << "[LLVM]: </IRGenerator>" << std::endl;
}

llvm::Value* IRGenerator::pop_tmp_v() {
    auto v = tmp_v;
    tmp_v = nullptr;
    return v;
}

llvm::Type* IRGenerator::pop_tmp_t() {
    llvm::Type* t = tmp_t;
    tmp_t = nullptr;
    return t;
}

void IRGenerator::visit(ast::Program *program) {
    if (shell.debug) std::cout << "[LLVM]: <Program>" << std::endl;

    for (auto u : program->variables) {
        u->accept(this);
    }
    for (auto u : program->routines) {
        u->accept(this);
    }
    if (shell.debug) std::cout << "[LLVM]: </Program>" << std::endl;
}

void IRGenerator::visit(ast::VariableDeclaration *var) {
    if (shell.debug) std::cout << "[LLVM]: <VariableDeclaration>" << std::endl;
    
    // visit Type and extract it
    var->dtype->accept(this);
    auto dtype = pop_tmp_t();
    
    auto v = builder.CreateAlloca(dtype, nullptr, var->name);
    
    // auto v = new llvm::AllocaInst(dtype, 0, var->name);

    // module->getOrInsertGlobal(var->name, dtype);
    // llvm::GlobalVariable *v = module->getNamedGlobal(var->name);

    // check type
    // if (var->dtype->getType() == ast::TypeEnum::Array ||
    //     var->dtype->getType() == ast::TypeEnum::Record) {
    //     var->dtype->accept(this);
    //     builder->CreateStore(pop_tmp_v(), v);
    // } else {
    if (var->iv != nullptr) {
        var->iv->accept(this);
        builder.CreateStore(pop_tmp_v(), v);
    }
    // }

    named_values[var->name] = v;
    tmp_v = v;

    if (shell.debug) std::cout << "[LLVM]: </VariableDeclaration>" << std::endl;
}

void IRGenerator::visit(ast::IntegerType *it) {
    tmp_t = llvm::Type::getInt64Ty(context);
}

void IRGenerator::visit(ast::IntegerLiteral *il) {
    tmp_v = llvm::ConstantInt::get(context, llvm::APInt(64, il->value));
}

void IRGenerator::visit(ast::BinaryExpression *exp) {
    if (shell.debug) std::cout << "[LLVM]: <BinaryExpression>" << std::endl;
    if (shell.debug) std::cout << "[LLVM]: </BinaryExpression>" << std::endl;
}

void IRGenerator::visit(ast::RoutineDeclaration* routine) {
    if (shell.debug) std::cout << "[LLVM]: <RoutineDeclaration>" << std::endl;

    // Types of the parameters
    std::vector<llvm::Type*> paramTypes;
    for (auto& param : routine->params) {
        param->dtype->accept(this);
        paramTypes.push_back(pop_tmp_t());
    }

    // The routine's return type
    llvm::Type* rtype = llvm::Type::getVoidTy(context);
    if (routine->rtype != nullptr) {
        routine->rtype->accept(this);
        rtype = pop_tmp_t();
    }

    // The function's type: (return type, parameter types, varargs?)
    llvm::FunctionType* ft = llvm::FunctionType::get(rtype, paramTypes, false);

    // The actual function from the type, local to this module, with the given name
    llvm::Function* fun = llvm::Function::Create(
        ft,
        llvm::Function::ExternalLinkage,
        routine->name, module.get()
    );

    // Give the parameters their names
    unsigned idx = 0;
    for (auto& arg : fun->args()) {
        arg.setName(routine->params[idx++]->name);
    }

    // Create a new basic block to start inserting statements into
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", fun);

    // Tell the builder that upcoming instructions should go into this block
    builder.SetInsertPoint(bb);

    // Record the function arguments in the named_values map
    named_values.clear();
    for (auto& arg : fun->args()) {
        named_values[arg.getName()] = &arg;
    }

    routine->body->accept(this);
    llvm::verifyFunction(*fun);
    tmp_v = fun;

    if (shell.debug) std::cout << "[LLVM]: </RoutineDeclaration>" << std::endl;
}

void IRGenerator::visit(ast::Body* body) {
    if (shell.debug) std::cout << "[LLVM]: <Body>" << std::endl;
    auto vars = body->variables;
    auto stmts = body->statements;

    // Parse tree pushed them in reverse order.
    std::reverse(vars.begin(), vars.end());
    std::reverse(stmts.begin(), stmts.end());

    // for (auto& type : body->types) {
    //     type->accept(this);
    // }
    for (auto& var : vars) {
        var->accept(this);
    }
    for (auto& statement : stmts) {
        statement->accept(this);
    }

    if (shell.debug) std::cout << "[LLVM]: </Body>" << std::endl;
}

void IRGenerator::visit(ast::ReturnStatement* stmt) {
    if (shell.debug) std::cout << "[LLVM]: <ReturnStatement>" << std::endl;
    llvm::Value* rval = nullptr;
    if (stmt->exp != nullptr) {
        stmt->exp->accept(this);
        rval = pop_tmp_v();
    }
    builder.CreateRet(rval);
    if (shell.debug) std::cout << "[LLVM]: </ReturnStatement>" << std::endl;
}

void IRGenerator::visit(ast::PrintStatement* stmt) {
    if (shell.debug) std::cout << "[LLVM]: <PrintStatement>" << std::endl;

    llvm::Value* val = nullptr;
    if (stmt->exp != nullptr) {
        stmt->exp->accept(this);
        val = pop_tmp_v();
    }
    
    // Format string for printf
    std::string fmt = "%lld\n"; // TODO: change based on expression type

    // Add function call code
    std::vector<llvm::Value*> tmp;
    tmp.push_back(builder.CreateGlobalStringPtr(llvm::StringRef(fmt), "_"));
    tmp.push_back(val);
    auto args = llvm::ArrayRef<llvm::Value*>(tmp);

    // Printf function callee
    llvm::FunctionCallee print = module->getOrInsertFunction(
        "printf",
        llvm::FunctionType::get(
            llvm::IntegerType::getInt32Ty(context),
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
            true
        )
    );
    
    // Create function call
    builder.CreateCall(print, args, "printfCall");

    if (shell.debug) std::cout << "[LLVM]: </PrintStatement>" << std::endl;
}
