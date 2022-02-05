#include <iostream>
#include <memory>
#include <map>
#include <vector>

namespace ast
{
	struct Node;
    struct Program;
    struct Variable;
}

namespace ast
{
	using namespace std;

	template <typename T> using ast_node = shared_ptr<T>; // Pointer to an AST node.

    struct Node {
        string type;
    };

    // Program is a map of variables to their values (for now)
    struct Program : Node {
        vector<ast_node<Variable> > variables;
    };

    // Variable has a dtype, name and an inital value (optional).
    struct Variable : Node {
        string dtype;
        string name;
        int int_val;
        bool bool_val;
        double real_val;

        Variable(string dtype, string name, int value) {
            this->dtype = dtype;
            this->name = name;
            this->int_val = value;
        }

        Variable(string dtype, string name, double value) {
            this->dtype = dtype;
            this->name = name;
            this->real_val = value;
        }

        Variable(string dtype, string name, bool value) {
            this->dtype = dtype;
            this->name = name;
            this->bool_val = value;
        }
    };
}
