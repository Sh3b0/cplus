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
    
    // TODO: deduce dtype of expressions to not get segfault here.
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

void IRGenerator::visit(ast::Identifier* id) {
    std::cout << "[LLVM]: <Identifier>" << std::endl;
   
    auto v = named_values[id->name];
    if (v == nullptr) {
        std::cerr << "[LLVM]: Error: " << id->name << " is not declared." << std::endl;
        return;
    }
 
    tmp_v = builder.CreateLoad(v, id->name);
    std::cout << "[LLVM]: </Identifier>" << std::endl;
}

void IRGenerator::visit(ast::UnaryExpression* exp) {
    if (shell.debug) std::cout << "[LLVM]: <UnaryExpression>" << std::endl;
    
    exp->operand->accept(this);
    llvm::Value *O = pop_tmp_v();

    switch (exp->op) {
        case ast::OperatorEnum::MINUS:
            if(O->getType()->isFloatingPointTy()) 
                builder.CreateFNeg(O, "negtmp");
            else
                tmp_v = builder.CreateNeg(O, "negtmp");
            break;

        case ast::OperatorEnum::NOT:
            tmp_v = builder.CreateNot(O, "subtmp");
            break;
    }

    if (shell.debug) std::cout << "[LLVM]: </UnaryExpression>" << std::endl;
}

void IRGenerator::visit(ast::BinaryExpression* exp) {
    if (shell.debug) std::cout << "[LLVM]: <BinaryExpression>" << std::endl;
    exp->lhs->accept(this);
    llvm::Value *L = pop_tmp_v();

    exp->rhs->accept(this);
    llvm::Value *R = pop_tmp_v();

    //-------------------------
    L->print(llvm::outs());
    switch (exp->op) {
        case ast::OperatorEnum::PLUS:
            std::cout << " + " << std::flush;
            break;
        case ast::OperatorEnum::MINUS:
            std::cout << " - " << std::flush;
            break;
        case ast::OperatorEnum::MUL:
            std::cout << " * " << std::flush;
            break;
        case ast::OperatorEnum::DIV:
            std::cout << " / " << std::flush;
            break;
        case ast::OperatorEnum::MOD:
            std::cout << " % " << std::flush;
            break;
        case ast::OperatorEnum::AND:
            std::cout << " & " << std::flush;
            break;
        case ast::OperatorEnum::OR:
            std::cout << " | " << std::flush;
            break;
        case ast::OperatorEnum::XOR:
            std::cout << " ^ " << std::flush;
            break;
    }
    R->print(llvm::outs());
    std::cout << std::endl;
    //-------------------------

    switch (exp->op) {
        case ast::OperatorEnum::PLUS:
            if(L->getType()->isFloatingPointTy() || R->getType()->isFloatingPointTy()) 
                tmp_v = builder.CreateFAdd(L, R, "addtmp");
            else
                tmp_v = builder.CreateAdd(L, R, "addtmp");
            break;

        case ast::OperatorEnum::MINUS:
            if(L->getType()->isFloatingPointTy() || R->getType()->isFloatingPointTy()) 
                tmp_v = builder.CreateFSub(L, R, "subtmp");
            else
                tmp_v = builder.CreateSub(L, R, "subtmp");
            break;
        
        case ast::OperatorEnum::MUL:
            if(L->getType()->isFloatingPointTy() || R->getType()->isFloatingPointTy()) 
                tmp_v = builder.CreateFMul(L, R, "multmp");
            else
                tmp_v = builder.CreateMul(L, R, "multmp");
            break;

        case ast::OperatorEnum::DIV:
            if(L->getType()->isFloatingPointTy() || R->getType()->isFloatingPointTy()) 
                tmp_v = builder.CreateFDiv(L, R, "divtmp");
            else
                tmp_v = builder.CreateSDiv(L, R, "divtmp");
            break;
        
        case ast::OperatorEnum::MOD:
            tmp_v = builder.CreateSRem(L, R, "remtmp");
            break;

        case ast::OperatorEnum::AND:
            tmp_v = builder.CreateAnd(L, R, "andtmp");
            break;

        case ast::OperatorEnum::OR:
            tmp_v = builder.CreateOr(L, R, "ortmp");
            break;
        
        case ast::OperatorEnum::XOR:
            tmp_v = builder.CreateXor(L, R, "xortmp");
            break;

        default:
            std::cerr << "[LLVM]: Error: Unknown operator" << std::endl;
            return;
    }
    if (shell.debug) std::cout << "[LLVM]: </BinaryExpression>" << std::endl;
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

    std::cout << "[LLVM]: vars = " << body->variables.size() << std::endl;
    std::cout << "[LLVM]: stmts = " << body->statements.size() << std::endl;

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

    stmt->exp->accept(this);
    llvm::Value* to_print = pop_tmp_v();
    
    // Format string for printf
    std::string fmt;
    if (to_print->getType()->isIntegerTy()) { // bool will also match here.
        fmt = "%lld\n";
    }
    else if (to_print->getType()->isFloatingPointTy()) {
        fmt = "%f\n";
    }
    else {
        std::cerr << "[LLVM]: Error: Cannot print " << std::flush;
        to_print->getType()->print(llvm::errs());
        return;

        // fmt = "%lld\n";
        // to_print = builder.CreateLoad(llvm::Type::getInt64Ty(context), to_print, "tmp");
        
        // to_print = pop_tmp_v();
        // to_print->getType()->print(llvm::outs());
        // to_print->print(llvm::outs());

        // if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::Poin>(to_print)) {
        //     if (CI->getBitWidth() <= 64) {
        //         std::cout << "I AM HERE" << std::endl;
        //         to_print = llvm::ConstantInt::get(context, llvm::APInt(64, CI->getSExtValue(), true));
        //     }
        // }
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

    if (shell.debug) std::cout << "[LLVM]: </PrintStatement>" << std::endl;
}
