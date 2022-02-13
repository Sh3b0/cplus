#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <variant>
#include <sstream>

namespace ast
{
    struct Node;
    struct Program;
    struct Literal;
    struct Variable;
    struct Routine;
    struct Array;
    struct Record;
    struct ExpressionNode;
    struct TypeNode;
}

namespace ast
{
    using namespace std;

    // Pointer to an AST node.
    template <typename T> using np = shared_ptr<T>;

    struct Node {
        string type;
    };

    struct Program : Node {
        map<string, np<Variable> > variables;
        vector<np<Routine> > routines;
        map<string, np<TypeNode> > types;
    };

    struct TypeNode : Node {
        variant<string, np<Array>, np<Record> > dtype;

        TypeNode( ) {
            this->dtype = "null";
        }

        TypeNode(string dtype) {
            this->dtype = dtype; 
        }

        TypeNode(np<Array> dtype) {
            this->dtype = dtype;
        }

        TypeNode(np<Record> dtype) {
            this->dtype = dtype;
        }
    };

    // Literal has a data type and value
    struct Literal : Node {
        np<TypeNode> dtype;
        variant<int, double, bool> value;

        Literal () {}

        Literal (np<TypeNode> dtype) {
            this->dtype = dtype;
        }

        Literal(np<TypeNode> dtype, int value) {
            this->dtype = dtype;
            this->value = value;
        }

        Literal(np<TypeNode> dtype, double value) {
            this->dtype = dtype;
            this->value = value;
        }

        Literal(np<TypeNode> dtype, bool value) {
            this->dtype = dtype;
            this->value = value;
        }

        np<Literal> add(np<Literal> that) {
            if (get<string>(this->dtype->dtype) == "integer" && get<string>(that->dtype->dtype) == "integer") {
                return make_shared<Literal>(make_shared<TypeNode>("integer"), get<int>(this->value) + get<int>(that->value));
            }
            // TODO: checks for other dtypes
            return nullptr;
        }

        friend std::ostream& operator<< (std::ostream& stream, const Literal& literal) {
            if (get<string>(literal.dtype->dtype) == "integer")
                stream << get<int>(literal.value);

            else if (get<string>(literal.dtype->dtype) == "real")
                stream << get<double>(literal.value);

            else if (get<string>(literal.dtype->dtype) == "boolean")
                stream << get<bool>(literal.value);
            
            else
                stream << "null";

            return stream;
        }
        
    };

    // Variable has a dtype, name and a value can be assigned to it.
    struct Variable : Node {
        np<TypeNode> dtype;
        string name;
        np<Literal> value;

        Variable(np<TypeNode> dtype, string name, np<Literal> value) {
            this->dtype = dtype;
            this->name = name;
            this->value = value;
        }
    };

    // Expression has a dtype and a value
    struct ExpressionNode : Node {
        np<TypeNode> dtype;
        np<Literal> value;

        ExpressionNode(np<TypeNode> dtype, np<Literal> value) {
            this->dtype = dtype;
            this->value = value;
        }
    };

    // Routine has a name, a list of parameters, a body, and a return type.
    struct Routine : Node {
        string name;
        vector<np<Variable> > params;

        Routine(string name){
            this->name = name;
        }
        // vector<np<Statement> > stmts;
        // Type rtype;
    };

    struct Array : Node {
        int size;
        np<TypeNode> dtype;
        Array () { }
        Array(int size, np<TypeNode> dtype){
            this->size = size;
            this->dtype = dtype;
        }
    };

    struct Record : Node {
        map<string, np<Variable> > variables;
        Record () { }
        Record(map<string, np<Variable> > variables){
            this->variables = variables;
        }
    };
    
}
