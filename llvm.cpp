#include "llvm.hpp"

#define RED         "\033[31m"
#define CYAN        "\033[36m"
#define YELLOW      "\033[33m"
#define RESET       "\033[0m"

#define GDEBUG(X)   if (shell.debug) std::cout << CYAN << X << RESET << std::endl;
#define GWARNING(X) std::cerr << YELLOW << "[LLVM]: [WARNING]: " << X << RESET << std::endl;
#define GERROR(X)   std::cerr << RED << "[LLVM]: [ERROR]: " << X << RESET << std::endl; std::exit(1);

#define BLOCK_B(X)                                           \
    if (shell.debug){                                        \
        std::cout << CYAN;                                   \
        for(int i=0; i<spaces; i++) {                        \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "<" << X << ">" << RESET << std::endl;  \
        spaces += 4;                                         \
    }

#define BLOCK_E(X)                                           \
    if (shell.debug){                                        \
        spaces -= 4;                                         \
        std::cout << CYAN;                                   \
        for(int i=0; i<spaces; i++) {                        \
            i % 4 ? std::cout << " " : std::cout << "|";     \
        }                                                    \
        std::cout << "</" << X << ">" << RESET << std::endl; \
    }

extern cplus::shell shell;

// Constructor
IRGenerator::IRGenerator() {
    module = std::make_unique<llvm::Module>(llvm::StringRef("ir.ll"), context);
    builder = std::make_unique<llvm::IRBuilder<>>(context);

    int_t = llvm::Type::getInt64Ty(context);
    real_t = llvm::Type::getDoubleTy(context);
    bool_t = llvm::Type::getInt1Ty(context);
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

llvm::Value *IRGenerator::pop_v() {
    auto v = tmp_v;
    tmp_v = nullptr;
    return v;
}

llvm::Value *IRGenerator::pop_p() {
    auto p = tmp_p;
    tmp_p = nullptr;
    return p;
}

llvm::Type *IRGenerator::pop_t() {
    llvm::Type *t = tmp_t;
    tmp_t = nullptr;
    return t;
}

void IRGenerator::visit(ast::Program *program) {
    BLOCK_B("Program")

    std::reverse(program->variables.begin(), program->variables.end());
    std::reverse(program->routines.begin(), program->routines.end());

    for (auto u : program->variables) {
        u->accept(this);
    }
    
    global_vars_pass = false;

    for (auto u : program->routines) {
        u->accept(this);
        is_first_routine = false;
    }

    BLOCK_E("Program")
}

// Defines a global, or stores a pointer to the created local var in location map.
void IRGenerator::visit(ast::VariableDeclaration *var) {
    BLOCK_B("VariableDeclaration")

    llvm::Type *dtype = nullptr;

    // var dtype is given
    if (var->dtype) {

        // dtype is an array
        if (var->dtype->getType() == ast::TypeEnum::ARRAY) {
            var->dtype->accept(this);       // array will be created by this visit,
            location[var->name] = pop_p();  // save a pointer to the array location for later access.
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
            if(var->iv) { // iv is given, deduce dtype 
                goto deduce;
            }
            else {
                var->dtype->accept(this);
                dtype = pop_t();
            }
        }

        // unknown dtype
        else {
            GERROR("Unsupported data type for variable" << var->name)
        }
    }

    // dtype is not given, deduce dtype from iv
    else {
        deduce:
        var->iv->accept(this);
        dtype = pop_t();
        
        if(!dtype) {
            GERROR("Cannot deduce variable dtype from initializer")
        }
    }
    
    // Variable being declared is global
    if(global_vars_pass) {

        // Create the global
        module->getOrInsertGlobal(var->name, dtype);
        auto g = module->getNamedGlobal(var->name);
        g->setLinkage(llvm::GlobalValue::ExternalLinkage);

        // global is not initialized, initialize it with default value
        if(!(var->iv)) {
            if(dtype == int_t) {
                g->setInitializer(llvm::ConstantInt::get(context, llvm::APInt(64, 0, true)));
            }
            else if(dtype == bool_t) {
                g->setInitializer(llvm::ConstantInt::get(context, llvm::APInt(1, 0, false)));
            }
            else if(dtype == real_t) {
                g->setInitializer(llvm::ConstantFP::get(context, llvm::APFloat(0.0)));
            }
            else {
                GERROR("Global arrays/records are not supported")
            }
        }

        // global is initialized, set initializer for it.
        else {
            var->iv->accept(this);
            if(dtype == int_t || dtype == bool_t) {
                if (auto init = llvm::dyn_cast<llvm::ConstantInt>(pop_v())) {
                    g->setInitializer(init);
                }
                else {
                    goto init_error;
                }
            }
            else if(dtype == real_t) {
                if (auto init = llvm::dyn_cast<llvm::ConstantFP>(pop_v())) {
                    g->setInitializer(init);
                }
                else {
                    goto init_error;
                }
            }
            else {
                init_error:
                GERROR("Global variable cannot be initialized with non-constant value")
            }
        }
    }

    // Variable being declared is local
    else {
        // Allocate space for the (primitive) variable
        auto p = builder->CreateAlloca(dtype, nullptr, var->name);

        // If an initial value was given, store it in the allocated space. 
        if (var->iv) {
            var->iv->accept(this);
            builder->CreateStore(pop_v(), p);
        }
        
        // Save var location for later reference
        location[var->name] = p;
    }

    BLOCK_E("VariableDeclaration")
}

// Sets tmp_p and optionally tmp_v and tmp_t
void IRGenerator::visit(ast::Identifier* id) {
    BLOCK_B("Identifier")

    auto global = module->getNamedGlobal(id->name);
    auto p = global ? global : location[id->name];

    if (!p) {
        GERROR(id->name << " is not declared.")
    }
    
    // accessing an array element
    if(id->idx) {
        id->idx->accept(this);

        // GetElementPointer (GEP) instruction will get the array element location. 
        tmp_p = builder->CreateGEP(p, pop_v());
    }

    // accessing a primitive or a record field
    else {
        tmp_p = p;
    }

    // If a value is stored there, load it, otherwise leave tmp_v and tmp_t as nullptrs.
    // Other visits such as PrintStatement should handle unassigned values.
    auto val = builder->CreateLoad(tmp_p, id->name);
    if(val) {
        tmp_v = val;
        tmp_t = tmp_v->getType();
    }
    
    BLOCK_E("Identifier")
}

// Sets tmp_v and tmp_t
void IRGenerator::visit(ast::UnaryExpression* exp) {
    BLOCK_B("UnaryExpression")
    
    exp->operand->accept(this);
    llvm::Value *O = pop_v();

    switch (exp->op) {
        case ast::OperatorEnum::MINUS:
            if(O->getType()->isFloatingPointTy()) {
                tmp_v = builder->CreateFNeg(O, "negtmp");
                tmp_t = real_t;
            }
            else {
                tmp_v = builder->CreateNeg(O, "negtmp");
                tmp_t = int_t;
            }
            break;

        case ast::OperatorEnum::NOT:
            tmp_v = builder->CreateNot(O, "nottmp");
            tmp_t = bool_t;
            break;
    }

    BLOCK_E("UnaryExpression")
}

// Sets tmp_v and tmp_t
void IRGenerator::visit(ast::BinaryExpression* exp) {
    BLOCK_B("BinaryExpression")

    exp->lhs->accept(this);
    llvm::Value *L = pop_v();

    exp->rhs->accept(this);
    llvm::Value *R = pop_v();

    // if(shell.debug) {
    //     L->print(llvm::outs());
    //     std:: cout << " " << op_to_str[exp->op] << " " << std::flush;
    //     R->print(llvm::outs());
    //     std::cout << std::endl;
    // }
    
    bool float_exp = false, bool_exp = false;
    if(L->getType()->isFloatingPointTy() && R->getType()->isIntegerTy()){
        float_exp = true;
        R = builder->CreateUIToFP(R, real_t);
    }
    else if(L->getType()->isIntegerTy() && R->getType()->isFloatingPointTy()){
        float_exp = true;
        L = builder->CreateUIToFP(L, real_t);
    }
    else if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy()){
        float_exp = true;
    }

    switch (exp->op) {
        case ast::OperatorEnum::PLUS:
            if(float_exp) tmp_v = builder->CreateFAdd(L, R, "addtmp");
            else tmp_v = builder->CreateAdd(L, R, "addtmp"); 
            break;

        case ast::OperatorEnum::MINUS:
            if(float_exp) tmp_v = builder->CreateFSub(L, R, "subtmp");
            else tmp_v = builder->CreateSub(L, R, "subtmp");
            break;
        
        case ast::OperatorEnum::MUL:
            if(float_exp) tmp_v = builder->CreateFMul(L, R, "multmp");
            else tmp_v = builder->CreateMul(L, R, "multmp");
            break;

        case ast::OperatorEnum::DIV:
            if(float_exp) tmp_v = builder->CreateFDiv(L, R, "divtmp");
            else tmp_v = builder->CreateSDiv(L, R, "divtmp");
            break;
        
        case ast::OperatorEnum::MOD:
            tmp_v = builder->CreateSRem(L, R, "remtmp");
            break;

        case ast::OperatorEnum::AND:
            bool_exp = true;
            tmp_v = builder->CreateAnd(L, R, "andtmp");
            break;

        case ast::OperatorEnum::OR:
            bool_exp = true;
            tmp_v = builder->CreateOr(L, R, "ortmp");
            break;
        
        case ast::OperatorEnum::XOR:
            bool_exp = true;
            tmp_v = builder->CreateXor(L, R, "xortmp");
            break;
        
        case ast::OperatorEnum::EQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpUEQ(L, R, "eqtmp");
            else tmp_v = builder->CreateICmpEQ(L, R, "eqtmp");
            break;

        case ast::OperatorEnum::NEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpUNE(L, R, "netmp");
            else tmp_v = builder->CreateICmpNE(L, R, "netmp");
            break;

        case ast::OperatorEnum::GT:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpUGT(L, R, "gttmp");
            else tmp_v = builder->CreateICmpSGT(L, R, "gttmp");
            break;

        case ast::OperatorEnum::LT:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpULT(L, R, "lttmp");
            else tmp_v = builder->CreateICmpSLT(L, R, "lttmp");
            break;
        
        case ast::OperatorEnum::GEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpUGE(L, R, "geqtmp");
            else tmp_v = builder->CreateICmpSGE(L, R, "geqtmp");
            break;
        
        case ast::OperatorEnum::LEQ:
            bool_exp = true;
            if(float_exp) tmp_v = builder->CreateFCmpULE(L, R, "leqtmp");
            else tmp_v = builder->CreateICmpSLE(L, R, "leqtmp");
            break;
        
        default:
            GERROR("Unknown operator encountered")
    }

    if(bool_exp) {
        tmp_t = bool_t;
    }
    else if(float_exp){
        tmp_t = real_t;
    }
    else {
        tmp_t = int_t;
    }

    BLOCK_E("BinaryExpression")
}

void IRGenerator::visit(ast::IntType *it) {
    tmp_t = int_t;
}

void IRGenerator::visit(ast::RealType *rt) {
    tmp_t = real_t;
}

void IRGenerator::visit(ast::BoolType *bt) {
    tmp_t = bool_t;
}

// Sets tmp_p (pointer to the beginning of array)
void IRGenerator::visit(ast::ArrayType *at) {
    BLOCK_B("ArrayType")

    if(global_vars_pass) {
        GERROR("Global arrays are not supported")
    }

    at->size->accept(this);
    auto size = pop_v();

    at->dtype->accept(this);
    auto dtype = pop_t();

    tmp_p = builder->CreateAlloca(dtype, size);

    BLOCK_E("ArrayType")
}

void IRGenerator::visit(ast::RecordType *rt) {
    BLOCK_B("RecordType")

    if(global_vars_pass) {
        GERROR("Global records are not supported")
    }

    for(auto field : rt->fields) {
        field->name = rt->name + "." + field->name;
        field->accept(this); // Creates a var with name "{rt->name}.{field->name}"
    }

    BLOCK_E("RecordType")
}

void IRGenerator::visit(ast::IntLiteral *il) {
    tmp_v = llvm::ConstantInt::get(context, llvm::APInt(64, il->value, true));
    tmp_t = int_t;
}

void IRGenerator::visit(ast::RealLiteral *rl) {
    tmp_v = llvm::ConstantFP::get(context, llvm::APFloat(rl->value));
    tmp_t = real_t;
}

void IRGenerator::visit(ast::BoolLiteral *bl) {
    tmp_v = llvm::ConstantInt::get(context, llvm::APInt(1, bl->value, false));
    tmp_t = bool_t;
}

// Sets tmp_v (the function pointer)
void IRGenerator::visit(ast::RoutineDeclaration* routine) {
    BLOCK_B("RoutineDeclaration")

    // Types of the parameters
    std::vector<llvm::Type*> paramTypes;
    for (auto& param : routine->params) {
        param->dtype->accept(this);
        paramTypes.push_back(pop_t());
    }

    // The routine's return type
    llvm::Type *rtype = llvm::Type::getVoidTy(context);
    if (routine->rtype) {
        routine->rtype->accept(this);
        rtype = pop_t();
    }

    // The function's type: (return type, parameter types, varargs?)
    llvm::FunctionType *ft = llvm::FunctionType::get(rtype, paramTypes, false);

    // The actual function from the type, local to this module, with the given name
    llvm::Function* fun = llvm::Function::Create(
        ft,
        llvm::Function::ExternalLinkage,
        routine->name,
        module.get()
    );

    // Give the parameters their names
    unsigned idx = 0;
    for (auto& arg : fun->args()) {
        arg.setName(routine->params[idx++]->name);
    }

    // Create a new basic block to start inserting statements into
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", fun);

    // Tell the builder that upcoming instructions should go into this block
    builder->SetInsertPoint(bb);

    // Record the function arguments in the location map
    // location.clear();
    for (auto& arg : fun->args()) {
        location[arg.getName()] = &arg;
    }

    // Create globals needed for PrintStatement
    if(is_first_routine) {
        fmt_lld = builder->CreateGlobalStringPtr(llvm::StringRef("%lld\n"), "fmt_lld");
        fmt_f = builder->CreateGlobalStringPtr(llvm::StringRef("%f\n"), "fmt_f");
        fmt_s = builder->CreateGlobalStringPtr(llvm::StringRef("%s\n"), "fmt_s");
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

    llvm::Value *rval = nullptr;
    if (stmt->exp) {
        stmt->exp->accept(this);
        rval = pop_v();
    }
    builder->CreateRet(rval);

    BLOCK_E("ReturnStatement")
}

void IRGenerator::visit(ast::PrintStatement* stmt) {
    BLOCK_B("PrintStatement")

    llvm::Value *to_print;
    llvm::Type *dtype;
    llvm::Constant *fmt;

    // Printing a constant string
    if(stmt->str != "") {
        fmt = fmt_s;
        to_print = builder->CreateGlobalStringPtr(llvm::StringRef(stmt->str), "str");
    }

    // Printing an expression
    else {
        stmt->exp->accept(this);

        // Get exp value
        to_print = pop_v();
        if(!to_print) {
            GERROR("Trying to print an unassigned value")
        }

        // Get exp dtype
        dtype = pop_t();
        if(!dtype) { // Happens when printing a constant value
            dtype = to_print->getType();
        }
        
        // Setting format string for printf depending on exp type
        if (dtype->isIntegerTy()) {
            fmt = fmt_lld;
        }
        else if (dtype->isFloatingPointTy()) {
            fmt = fmt_f;
        }
        else {
            std::cerr << RED << "[LLVM]: [ERROR]: Cannot print " << RESET << std::flush;
            to_print->print(llvm::errs());
            std::exit(1);
        }
    }

    // Add function call code
    std::vector<llvm::Value*> tmp;
    tmp.push_back(fmt);
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
    builder->CreateCall(print, args, "printfCall");

    BLOCK_E("PrintStatement")
}

void IRGenerator::visit(ast::AssignmentStatement* stmt) {
    BLOCK_B("AssignmentStatement")

    // id_loc is a pointer to the modifiable_primary to be accessed
    stmt->id->accept(this);
    auto id_loc = pop_p();

    // exp is a Value* containing the new data
    stmt->exp->accept(this);
    auto exp = pop_v();

    auto lhs_t = builder->CreateLoad(id_loc)->getType();
    auto rhs_t = exp->getType();

    if(lhs_t == int_t && rhs_t == real_t) { // real -> int
        exp = builder->CreateFPToSI(exp, lhs_t, "intcast");
    }
    else if(lhs_t == bool_t && rhs_t == real_t) { // real -> bool
        exp = builder->CreateFPToSI(exp, lhs_t, "boolcast");
    }
    else if(lhs_t == real_t && rhs_t == int_t) { // int -> real
        exp = builder->CreateSIToFP(exp, lhs_t, "fpcast");
    }
    else if(lhs_t == bool_t && rhs_t == int_t) { // int -> bool
        exp = builder->CreateICmpNE(exp, llvm::ConstantInt::get(context, llvm::APInt(64, 0)), "boolcast");
    }
    else if(lhs_t == int_t && rhs_t == bool_t) { // bool -> int
        exp = builder->CreateIntCast(exp, lhs_t, false);
    }
    else if(lhs_t == real_t && rhs_t == bool_t) { // bool -> real
        exp = builder->CreateFPCast(exp, lhs_t);
    }
    else if(lhs_t != rhs_t) {
        std::cerr << RED << "[LLVM]: [ERROR]: Unsupported conversion: ";
        lhs_t->print(llvm::errs());
        std::cerr << " -> ";
        rhs_t->print(llvm::errs());
        std::cerr << std::endl;
        std::exit(1);
    }
    
    builder->CreateStore(exp, id_loc);

    BLOCK_E("AssignmentStatement")
}

void IRGenerator::visit(ast::IfStatement* stmt) {
    BLOCK_B("IfStatement")

    stmt->cond->accept(this);
    llvm::Value *cond = pop_v();

    if(!cond->getType()->isIntegerTy() ||
        !(cond->getType()->getIntegerBitWidth() == 64 ||
        cond->getType()->getIntegerBitWidth() == 1)) {
        GERROR("condition expression is not of integer or boolean type")
    }

    // If cond is of type int64, compare it to 0 to create bool.
    if(cond->getType()->getIntegerBitWidth() == 64){
        cond = builder->CreateICmpNE(cond, llvm::ConstantInt::get(context, llvm::APInt(64, 0)), "cond");
    }

    // Get the current function
    llvm::Function* func = builder->GetInsertBlock()->getParent();

    // Create blocks for the then, else, endif.
    llvm::BasicBlock* then_block = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock* else_block = stmt->else_body ? llvm::BasicBlock::Create(context, "else") : nullptr;
    llvm::BasicBlock* endif = llvm::BasicBlock::Create(context, "endif");

    // Create the conditional statement
    builder->CreateCondBr(cond, then_block, else_block ? else_block : endif);

    // Then block
    {
        builder->SetInsertPoint(then_block);
        stmt->then_body->accept(this);
        builder->CreateBr(endif);
        
        then_block = builder->GetInsertBlock();
    }
    
    // Else block
    if(else_block) {
        func->getBasicBlockList().push_back(else_block);

        builder->SetInsertPoint(else_block);
        stmt->else_body->accept(this);
        builder->CreateBr(endif);

        else_block = builder->GetInsertBlock();
    }

    // Endif
    {
        func->getBasicBlockList().push_back(endif);
        builder->SetInsertPoint(endif);
    }
    
    BLOCK_E("IfStatement")
}
