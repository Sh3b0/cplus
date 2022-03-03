#include "llvm.hpp"

extern cplus::shell shell;

// Constructor
IRGenerator::IRGenerator() : builder(context) {
    module = std::make_unique<llvm::Module>(llvm::StringRef("ir.ll"), context);
    std::cout << "\e[1m\033[35m"; // Bold MAGENTA Text
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
    spaces += 1;
    for (auto u : program->variables) {
        u->accept(this);
    }
    for (auto u : program->routines) {
        u->accept(this);
    }
    spaces -= 1;
    if (shell.debug) std::cout << "[LLVM]: </Program>" << std::endl;
}

void IRGenerator::visit(ast::VariableDeclaration *var) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<VariableDeclaration>" << std::endl;
   
    // visit Type and extract it
    llvm::Type* dtype = nullptr;
    if(var->dtype != nullptr) {
        var->dtype->accept(this);
        dtype = pop_tmp_t();
    }
    else {
        if (var->iv == nullptr) {
            std::cerr << "[LLVM]: Error: dtype not explicitly stated and no initial value given." << std::endl;
            return;
        }
        var->iv->accept(this);
        dtype = pop_tmp_t();
        if(dtype == nullptr) {
            std::cerr << "[LLVM]: Error: Cannot deduce dtype for initializer" << std::endl;
        }
    }

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
    // tmp_t = dtype;

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</VariableDeclaration>" << std::endl;
}

void IRGenerator::visit(ast::Identifier* id) {
    std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<Identifier>" << std::endl;

    auto v = named_values[id->name];
    if (v == nullptr) {
        std::cerr << "[LLVM]: Error: " << id->name << " is not declared." << std::endl;
        return;
    }
 
    tmp_v = builder.CreateLoad(v, id->name);
    tmp_t = tmp_v->getType();
    std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</Identifier>" << std::endl;
}

void IRGenerator::visit(ast::UnaryExpression* exp) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<UnaryExpression>" << std::endl;
    
    exp->operand->accept(this);
    llvm::Value *O = pop_tmp_v();

    switch (exp->op) {
        case ast::OperatorEnum::MINUS:
            if(O->getType()->isFloatingPointTy()) {
                tmp_v = builder.CreateFNeg(O, "negtmp");
                tmp_t = llvm::Type::getDoubleTy(context);
            }
            else {
                tmp_v = builder.CreateNeg(O, "negtmp");
                tmp_t = llvm::Type::getInt64Ty(context);
            }
            break;

        case ast::OperatorEnum::NOT:
            tmp_v = builder.CreateNot(O, "nottmp");
            tmp_t = llvm::Type::getInt1Ty(context);
            break;
    }

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</UnaryExpression>" << std::endl;
}

void IRGenerator::visit(ast::BinaryExpression* exp) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<BinaryExpression>" << std::endl;
    exp->lhs->accept(this);
    llvm::Value *L = pop_tmp_v();

    exp->rhs->accept(this);
    llvm::Value *R = pop_tmp_v();

    // if(shell.debug) {
    //     L->print(llvm::outs());
    //     std:: cout << " " << op_to_str[exp->op] << " " << std::flush;
    //     R->print(llvm::outs());
    //     std::cout << std::endl;
    // }
    
    bool float_exp = false, bool_exp = false;
    if(L->getType()->isFloatingPointTy() && R->getType()->isIntegerTy()){
        float_exp = true;
        R = builder.CreateUIToFP(R, llvm::Type::getDoubleTy(context));
    }
    else if(L->getType()->isIntegerTy() && R->getType()->isFloatingPointTy()){
        float_exp = true;
        L = builder.CreateUIToFP(L, llvm::Type::getDoubleTy(context));
    }
    else if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy()){
        float_exp = true;
    }

    switch (exp->op) {
        case ast::OperatorEnum::PLUS:
            if(float_exp) tmp_v = builder.CreateFAdd(L, R, "addtmp");
            else tmp_v = builder.CreateAdd(L, R, "addtmp"); 
            break;

        case ast::OperatorEnum::MINUS:
            if(float_exp) tmp_v = builder.CreateFSub(L, R, "subtmp");
            else tmp_v = builder.CreateSub(L, R, "subtmp");
            break;
        
        case ast::OperatorEnum::MUL:
            if(float_exp) tmp_v = builder.CreateFMul(L, R, "multmp");
            else tmp_v = builder.CreateMul(L, R, "multmp");
            break;

        case ast::OperatorEnum::DIV:
            if(float_exp) tmp_v = builder.CreateFDiv(L, R, "divtmp");
            else tmp_v = builder.CreateSDiv(L, R, "divtmp");
            break;
        
        case ast::OperatorEnum::MOD:
            tmp_v = builder.CreateSRem(L, R, "remtmp");
            break;

        case ast::OperatorEnum::AND:
            bool_exp = true;
            tmp_v = builder.CreateAnd(L, R, "andtmp");
            break;

        case ast::OperatorEnum::OR:
            bool_exp = true;
            tmp_v = builder.CreateOr(L, R, "ortmp");
            break;
        
        case ast::OperatorEnum::XOR:
            bool_exp = true;
            tmp_v = builder.CreateXor(L, R, "xortmp");
            break;
        
        case ast::OperatorEnum::EQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpUEQ(L, R, "eqtmp");
            else tmp_v = builder.CreateICmpEQ(L, R, "eqtmp");
            break;

        case ast::OperatorEnum::NEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpUNE(L, R, "netmp");
            else tmp_v = builder.CreateICmpNE(L, R, "netmp");
            break;

        case ast::OperatorEnum::GT:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpUGT(L, R, "gttmp");
            else tmp_v = builder.CreateICmpSGT(L, R, "gttmp");
            break;

        case ast::OperatorEnum::LT:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpULT(L, R, "lttmp");
            else tmp_v = builder.CreateICmpSLT(L, R, "lttmp");
            break;
        
        case ast::OperatorEnum::GEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpUGE(L, R, "geqtmp");
            else tmp_v = builder.CreateICmpSGE(L, R, "geqtmp");
            break;
        
        case ast::OperatorEnum::LEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder.CreateFCmpULE(L, R, "leqtmp");
            else tmp_v = builder.CreateICmpSLE(L, R, "leqtmp");
            break;
        
        default:
            std::cerr << "[LLVM]: Error: Unknown operator" << std::endl;
            return;
    }

    if(bool_exp) {
        tmp_t = llvm::Type::getInt1Ty(context);
    }
    else if(float_exp){
        tmp_t = llvm::Type::getDoubleTy(context);
    }
    else {
        tmp_t = llvm::Type::getInt64Ty(context);
    }

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</BinaryExpression>" << std::endl;
}

void IRGenerator::visit(ast::IntType *it) {
    tmp_t = llvm::Type::getInt64Ty(context);
}

void IRGenerator::visit(ast::RealType *rt) {
    tmp_t = llvm::Type::getDoubleTy(context);
}

void IRGenerator::visit(ast::BoolType *bt) {
    tmp_t = llvm::Type::getInt1Ty(context);
}

void IRGenerator::visit(ast::IntLiteral *il) {
    tmp_v = llvm::ConstantInt::get(context, llvm::APInt(64, il->value, true));
}

void IRGenerator::visit(ast::RealLiteral *rl) {
    tmp_v = llvm::ConstantFP::get(context, llvm::APFloat(rl->value));
}

void IRGenerator::visit(ast::BoolLiteral *bl) {
    tmp_v = llvm::ConstantInt::get(context, llvm::APInt(1, bl->value, true));
}

void IRGenerator::visit(ast::RoutineDeclaration* routine) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<RoutineDeclaration>" << std::endl;

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

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</RoutineDeclaration>" << std::endl;
}

void IRGenerator::visit(ast::Body* body) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<Body>" << std::endl;
    auto vars = body->variables;
    auto stmts = body->statements;

    // std::cout << "[LLVM]: vars = " << body->variables.size() << std::endl;
    // std::cout << "[LLVM]: stmts = " << body->statements.size() << std::endl;

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

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</Body>" << std::endl;
}

void IRGenerator::visit(ast::ReturnStatement* stmt) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<ReturnStatement>" << std::endl;
    llvm::Value* rval = nullptr;
    if (stmt->exp != nullptr) {
        stmt->exp->accept(this);
        rval = pop_tmp_v();
    }
    builder.CreateRet(rval);
    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</ReturnStatement>" << std::endl;
}

void IRGenerator::visit(ast::PrintStatement* stmt) {
    if (shell.debug) std::cout << "[LLVM]: " << std::string(spaces++, ' ') << "<PrintStatement>" << std::endl;

    stmt->exp->accept(this);
    llvm::Value* to_print = pop_tmp_v();
    llvm::Type* dtype = pop_tmp_t();

    if(dtype == nullptr) { // Printing a constant
        dtype = to_print->getType();
    }
    
    // Format string for printf
    std::string fmt;
    if (dtype->isIntegerTy()) {
        fmt = "%lld\n";
    }
    else if (dtype->isFloatingPointTy()) {
        fmt = "%f\n";
    }
    else {
        std::cerr << "[LLVM]: Error: Cannot print " << std::flush;
        to_print->print(llvm::errs());
        return;
    }
    
    // Add function call code
    std::vector<llvm::Value*> tmp;
    tmp.push_back(builder.CreateGlobalStringPtr(llvm::StringRef(fmt), "_"));
    tmp.push_back(to_print);
    auto args = llvm::ArrayRef<llvm::Value*>(tmp);

    // Printf function callee
    llvm::FunctionCallee print = module->getOrInsertFunction(
        "printf",
        llvm::FunctionType::get(
            llvm::IntegerType::getInt64Ty(context),
            llvm::PointerType::get(llvm::Type::getInt8Ty(context), 0),
            true
        )
    );
    
    // Create function call
    builder.CreateCall(print, args, "printfCall");

    if (shell.debug) std::cout << "[LLVM]: " << std::string(--spaces, ' ') << "</PrintStatement>" << std::endl;
}
