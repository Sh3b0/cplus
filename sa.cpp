#include <iostream>
#include "sa.hpp"
#include "ast.hpp"

using namespace cplus;
using namespace std;
using namespace ast;

extern np<Program> program;

sa::sa() {}

bool sa::checkRecordAccess(string id1, string id2)
{
    if(program->types[id1] == nullptr){
        cout << "[SA]: " << id1 << " is not a user type\n";
        return false; 
    }
    if(!holds_alternative<np<Record>>(program->types[id1]->dtype)){
        cout << "[SA]: " << id1 << " is not a record\n";
        return false;
    }
    if(get<np<Record>>(program->types[id1]->dtype)->variables[id2] == nullptr){
        cout << "[SA]: " << id2 << " is not a member of " << id1 << "\n";
        return false;
    }
    return true;
}

bool sa::checkArrayAccess(string id, int idx)
{
    if(program->types[id] == nullptr){
        cout << "[SA]: " << id << " is not a user type\n";
        return false; 
    }
    if(!holds_alternative<np<Array>>(program->types[id]->dtype)){
        cout << "[SA]: " << id << " is not an array\n";
        return false;
    }
    int arrlen = get<np<Array>>(program->types[id]->dtype)->size;
    if(arrlen < idx){
        cout << "[SA]: array \"" << id << "\" index out of bounds: ";
        cout << idx << " > " << arrlen << '\n';
        return false;
    }
    return true;
}

bool sa::checkVariableAccess(string id)
{
    if(program->variables[id] == nullptr){
        cout << "[SA]: " << id << " is not a declared variable\n";
        return false; 
    }
    return true;
}

bool sa::checkRoutineCall(string id, int nargs)
{
    if(program->routines[id] == nullptr){
        cout << "[SA]: " << id << " is not callable\n";
        return false; 
    }
    int args = program->routines[id]->params.size();
    if(nargs != args) {
        cout << "[SA]: arity mismatch while calling " << id << ". Expected: ";
        cout << args << ", found: " << nargs << "\n";
        return false; 
    }
    return true;
}
