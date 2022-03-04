#include "llvm.hpp"

#define RED         "\033[31m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define RESET       "\033[0m"

#define GDEBUG(X)   if (shell.debug) std::cout << CYAN << X << RESET << std::endl;
#define GWARNING(X) std::cerr << RED << "[LLVM]: [ERROR]: " << X << RESET << std::endl;
#define GERROR(X)   std::cerr << YELLOW << "[LLVM]: [WARNING]: " << X << RESET << std::endl; 

#define BLOCK_B(X) \
    if (shell.debug){ \
        std::cout << CYAN; \
        for(int i=0; i<spaces; i++) i % 4 ? std::cout << " " : std::cout << "|"; \
        std::cout << "<" << X << ">" << RESET << std::endl; \
        spaces += 4; \
    }

#define BLOCK_E(X) \
    if (shell.debug){ \
        spaces -= 4; \
        std::cout << CYAN; \
        for(int i=0; i<spaces; i++) i % 4 ? std::cout << " " : std::cout << "|"; \
        std::cout << "</" << X << ">" << RESET << std::endl; \
    }

extern cplus::shell shell;

// Constructor
IRGenerator::IRGenerator() : builder(context) {
    module = std::make_unique<llvm::Module>(llvm::StringRef("ir.ll"), context);
}

// Emits IR code as "ir.ll"
void IRGenerator::generate() {
    auto CPU = "generic";
    auto Features = "";

    module->setTargetTriple(llvm::sys::getDefaultTargetTriple());

    std::string msg;
    llvm::raw_string_ostream out(msg);
    if(llvm::verifyModule(*this->module, &out)) {
        GWARNING(out.str())
    }

    FILE* f = fopen("ir.ll", "w");
    llvm::raw_fd_ostream outfile(fileno(f), true);
    module->print(outfile, nullptr);
}

llvm::Value* IRGenerator::pop_tmp_v() {
    auto v = tmp_v;
    tmp_v = nullptr;
    return v;
}

llvm::Value* IRGenerator::pop_tmp_p() {
    auto p = tmp_p;
    tmp_p = nullptr;
    return p;
}

llvm::Type* IRGenerator::pop_tmp_t() {
    llvm::Type* t = tmp_t;
    tmp_t = nullptr;
    return t;
}

void IRGenerator::visit(ast::Program *program) {
    BLOCK_B("Program")
    for (auto u : program->variables) {
        u->accept(this);
    }
    for (auto u : program->routines) {
        u->accept(this);
    }
    BLOCK_E("Program")
}

// Stores a pointer to the created var in location map.
void IRGenerator::visit(ast::VariableDeclaration *var) {
    BLOCK_B("VariableDeclaration")

    llvm::Type* dtype = nullptr;

    // var dtype is given
    if (var->dtype != nullptr) {

        // dtype is an array
        if (var->dtype->getType() == ast::TypeEnum::ARRAY) {
            var->dtype->accept(this);           // array will be created by this visit,
            location[var->name] = pop_tmp_p();  // save a pointer to the array location for later access.
            BLOCK_E("VariableDeclaration")
            return;
        }

        // dtype is a record
        else if (var->dtype->getType() == ast::TypeEnum::RECORD) {
            // downcasting np<Type> --> np<RecordType>
            auto t = std::dynamic_pointer_cast<ast::RecordType>(var->dtype); 
            t->name = var->name;
            t->accept(this);  // record fields will be created by this visit with "{var->name}." prefix.
            BLOCK_E("VariableDeclaration")
            return;
        }

        // dtype is primitive
        else if (var->dtype->getType() == ast::TypeEnum::INT  ||
            var->dtype->getType() == ast::TypeEnum::REAL ||
            var->dtype->getType() == ast::TypeEnum::BOOL) { 
            var->dtype->accept(this);
            dtype = pop_tmp_t();
        }

        // unknown dtype
        else {
            std::cerr << RED << "[LLVM]: Error: Unsupported data type for variable " << var->name << std::endl;
            std::exit(1);
        }
    }

    // dtype is not given, deduce dtype from iv
    else {
        var->iv->accept(this);
        dtype = pop_tmp_t();
        
        if(dtype == nullptr) {
            std::cerr << RED << "[LLVM]: Error: Cannot deduce variable dtype from initializer" << std::endl;
            std::exit(1);
        }
    }
    
    // module->getOrInsertGlobal(var->name, dtype);
    // llvm::GlobalVariable *v = module->getNamedGlobal(var->name);

    // Allocate space for the (primitive) variable
    auto p = builder.CreateAlloca(dtype, nullptr, var->name);

    // If an initial value was given, store it in the allocated space. 
    if (var->iv != nullptr) {
        var->iv->accept(this);
        builder.CreateStore(pop_tmp_v(), p);
    }
    
    // Save var location for later reference
    location[var->name] = p;

    BLOCK_E("VariableDeclaration")
}

// Sets tmp_p and optionally tmp_v and tmp_t
void IRGenerator::visit(ast::Identifier* id) {
    BLOCK_B("Identifier")

    auto p = location[id->name];
    if (p == nullptr) {
        std::cerr << RED << "[LLVM]: Error: " << id->name << " is not declared." << std::endl;
        std::exit(1);
    }
    
    // accessing an array element
    if(id->idx != nullptr) {
        id->idx->accept(this);

        // GetElementPointer (GEP) instruction will get the array element location. 
        tmp_p = builder.CreateGEP(p, pop_tmp_v());
    }

    // accessing a primitive or a record field
    else {
        tmp_p = p;
    }

    // If a value is stored there, load it, otherwise leave tmp_v and tmp_t as nullptrs.
    // Other visits such as PrintStatement should handle unassigned values.
    auto val = builder.CreateLoad(tmp_p, id->name);
    if(val != nullptr) {
        tmp_v = val;
        tmp_t = tmp_v->getType();
    }
    
    BLOCK_E("Identifier")
}

// Sets tmp_v and tmp_t
void IRGenerator::visit(ast::UnaryExpression* exp) {
    BLOCK_B("UnaryExpression")
    
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

    BLOCK_E("UnaryExpression")
}

// Sets tmp_v and tmp_t
void IRGenerator::visit(ast::BinaryExpression* exp) {
    BLOCK_B("BinaryExpression")

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
            std::cerr << RED << "[LLVM]: Error: Unknown operator" << std::endl;
            std::exit(1);
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

    BLOCK_E("BinaryExpression")
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

// Sets tmp_p (pointer to the beginning of array)
void IRGenerator::visit(ast::ArrayType *at) {
    BLOCK_B("ArrayType")

    at->size->accept(this);
    auto size = pop_tmp_v();

    at->dtype->accept(this);
    auto dtype = pop_tmp_t();

    tmp_p = builder.CreateAlloca(dtype, size);

    BLOCK_E("ArrayType")
}

void IRGenerator::visit(ast::RecordType *rt) {
    BLOCK_B("RecordType")

    for(auto field : rt->fields) {
        field->name = rt->name + "." + field->name;
        field->accept(this); // Creates a var with name "{rt->name}.{field->name}"
    }

    BLOCK_E("RecordType")
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

// Sets tmp_v (the function pointer)
void IRGenerator::visit(ast::RoutineDeclaration* routine) {
    BLOCK_B("RoutineDeclaration")

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

    // Record the function arguments in the location map
    location.clear();
    for (auto& arg : fun->args()) {
        location[arg.getName()] = &arg;
    }

    routine->body->accept(this);
    llvm::verifyFunction(*fun);
    tmp_v = fun;

    BLOCK_E("RoutineDeclaration")
}

void IRGenerator::visit(ast::Body* body) {
    BLOCK_B("Body")

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
    for (auto& stmt : stmts) {
        stmt->accept(this);
    }

    BLOCK_E("Body")
}

void IRGenerator::visit(ast::ReturnStatement* stmt) {
    BLOCK_B("ReturnStatement")

    llvm::Value* rval = nullptr;
    if (stmt->exp != nullptr) {
        stmt->exp->accept(this);
        rval = pop_tmp_v();
    }
    builder.CreateRet(rval);

    BLOCK_E("ReturnStatement")
}

void IRGenerator::visit(ast::PrintStatement* stmt) {
    BLOCK_B("PrintStatement")

    stmt->exp->accept(this);
    llvm::Value* to_print = pop_tmp_v();

    if(to_print == nullptr) {
        std::cerr << RED << "[LLVM]: Printing an unassigned value" << std::endl;
        std::exit(1);
    }

    llvm::Type* dtype = pop_tmp_t();

    // If printing a constant
    if(dtype == nullptr) {
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
        std::cerr << RED << "[LLVM]: Error: Cannot print " << std::flush;
        to_print->print(llvm::errs());
        std::exit(1);
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

    BLOCK_E("PrintStatement")
}

void IRGenerator::visit(ast::AssignmentStatement* stmt) {
    BLOCK_B("AssignmentStatement")

    stmt->id->accept(this);
    auto id_loc = pop_tmp_p();

    stmt->exp->accept(this);
    auto exp = pop_tmp_v();

    // id_loc is a pointer to the modifiable_primary to be accessed
    // exp is a Value* containing the new data

    // TODO: type checks and casts.

    // auto lhs_t = builder.CreateLoad(id_loc, "id*")->getType();
    // auto rhs_t = exp->getType();
    
    // if(lhs_t == llvm::Type::getInt64Ty(context) && rhs_t == llvm::Type::getDoubleTy(context)) {
    //     exp = builder.CreateCast(llvm::CastInst::get)
    // }

    builder.CreateStore(exp, id_loc);

    BLOCK_E("AssignmentStatement")
}

void IRGenerator::visit(ast::IfStatement* stmt) {
    BLOCK_B("IfStatement")

    stmt->cond->accept(this);
    llvm::Value* cond = pop_tmp_v();

    if (cond->getType()->isFloatingPointTy()) {
        std::cerr << RED << "[LLVM]: Error: If condition cannot be a real expression" << std::endl;
        exit(1);
    }

    // If cond is of IntegerType, compare it to 0 to create bool.
    if(cond->getType()->getIntegerBitWidth() == 64){
        cond = builder.CreateICmpNE(cond, llvm::ConstantInt::get(context, llvm::APInt(64, 0)), "cond");
    }

    // Get the current function
    llvm::Function* func = builder.GetInsertBlock()->getParent();

    // Create blocks for the then, else, endif.
    llvm::BasicBlock* then_block = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock* else_block = stmt->else_body ? llvm::BasicBlock::Create(context, "else") : nullptr;
    llvm::BasicBlock* endif = llvm::BasicBlock::Create(context, "endif");

    // Create the conditional statement
    builder.CreateCondBr(cond, then_block, else_block ? else_block : endif);

    // Then block
    {
        builder.SetInsertPoint(then_block);
        stmt->then_body->accept(this);
        builder.CreateBr(endif);
        
        then_block = builder.GetInsertBlock();
    }
    
    // Else block
    if(else_block) {
        func->getBasicBlockList().push_back(else_block);

        builder.SetInsertPoint(else_block);
        stmt->else_body->accept(this);
        builder.CreateBr(endif);

        else_block = builder.GetInsertBlock();
    }

    // Endif
    {
        func->getBasicBlockList().push_back(endif);
        builder.SetInsertPoint(endif);
    }
    
    BLOCK_E("IfStatement")
}
