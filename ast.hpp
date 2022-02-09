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
    struct ExpressionNode;
    struct Routine;
}

namespace ast
{
    using namespace std;

    // Pointer to an AST node.
    template <typename T> using ast_node = shared_ptr<T>;

    struct Node {
        string type;
    };

    // Program is a list of variables and routines
    struct Program : Node {
        vector<ast_node<Variable> > variables;
        vector<ast_node<Routine> > routines;
        
        // TODO: create a struct Type.
        // it will be used instead of string literals "integer", "real", etc
        // and to represent arrays and records.
        
        // TODO: use this map to store type aliases.
        // map<string, ast_node<Type> > types;
    };

    // Literal has a data type and value
    struct Literal : Node {
        string dtype;
        variant<int, double, bool> value;

        Literal () {
            this->dtype = "null";
        }

        Literal(int value) {
            this->dtype = "integer";
            this->value = value;
        }

        Literal(double value) {
            this->dtype = "real";
            this->value = value;
        }

        Literal(bool value) {
            this->dtype = "boolean";
            this->value = value;
        }

        friend std::ostream& operator<< (std::ostream& stream, const Literal& literal) {
            if (literal.dtype == "integer") stream << get<int>(literal.value);
            else if (literal.dtype == "real") stream << get<double>(literal.value);
            else if (literal.dtype == "boolean") stream << get<bool>(literal.value);
            else stream << "[AST] NO_VALUE";
            return stream;
        }
        
    };

    // Variable has a dtype, name and an inital value (optional).
    struct Variable : Node {
        string dtype;
        string name;
        Literal value;

        Variable(string dtype, string name, ast_node<Literal> value) {
            this->dtype = dtype;
            this->name = name;
            this->value = *value;
        }
    };

    // Expression has a dtype and a value
    struct ExpressionNode : Node {
        string dtype;
        Literal value;

        ExpressionNode(string dtype, ast_node<Literal> value) {
            this->dtype = dtype;
            this->value = *value;
        }
    };

    // Routine has a name, a list of parameters, a body, and a return type.
    struct Routine : Node {
        string name;
        vector<ast_node<Variable> > params;
        // vector<ast_node<Statement> > stmts;
        // Type rtype;
    };

}
