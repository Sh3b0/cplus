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
    enum Primitive { NIL, INTEGER, REAL, BOOLEAN };
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
        map<string, np<Variable>> variables;
        map<string, np<Routine>> routines;
        map<string, np<TypeNode>> types;
    };

    struct TypeNode : Node {
        variant<np<Primitive>, np<Array>, np<Record> > dtype;

        TypeNode( ) {
            this->dtype = make_shared<Primitive>(NIL);
        }

        TypeNode(variant<np<Primitive>, np<Array>, np<Record> > dtype) {
            this->dtype = dtype; 
        }
        
        friend ostream& operator<< (ostream& stream, const TypeNode& t) {
            std::visit([&](variant<np<Primitive>, np<Array>, np<Record> >&& arg){
                if(holds_alternative<np<Primitive>>(arg)) stream << "Primitive";
                else if(holds_alternative<np<Array>>(arg)) stream << "Array";
                else if(holds_alternative<np<Record>>(arg)) stream << "Record";
            }, t.dtype);
            return stream;
        }
    };

    struct Literal : Node {
        np<TypeNode> dtype;
        variant<int, double, bool> value;

        Literal () {}

        Literal (np<TypeNode> dtype) {
            this->dtype = dtype;
        }

        Literal(np<TypeNode> dtype, variant<int, double, bool> value) {
            this->dtype = dtype;
            this->value = value;
        }


        /*

            Arithmatics

        */
        np<Literal> add(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == INTEGER && *get<np<Primitive>>(that->dtype->dtype) == INTEGER) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(INTEGER)), get<int>(this->value) + get<int>(that->value));
            }
            if ((*get<np<Primitive>>(this->dtype->dtype) == INTEGER || *get<np<Primitive>>(this->dtype->dtype) == REAL) && 
            (*get<np<Primitive>>(that->dtype->dtype) == INTEGER || *get<np<Primitive>>(that->dtype->dtype) == REAL)) {
                auto v1 = (*get<np<Primitive>>(this->dtype->dtype) == INTEGER) ? get<int>(this->value) : get<double>(this->value);
                auto v2 = (*get<np<Primitive>>(that->dtype->dtype) == INTEGER) ? get<int>(that->value) : get<double>(that->value);
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(REAL)), v1 + v2);
            }
            return nullptr;
        }
        np<Literal> sub(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == INTEGER && *get<np<Primitive>>(that->dtype->dtype) == INTEGER) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(INTEGER)), get<int>(this->value) - get<int>(that->value));
            }
            if ((*get<np<Primitive>>(this->dtype->dtype) == INTEGER || *get<np<Primitive>>(this->dtype->dtype) == REAL) && 
            (*get<np<Primitive>>(that->dtype->dtype) == INTEGER || *get<np<Primitive>>(that->dtype->dtype) == REAL)) {
                auto v1 = (*get<np<Primitive>>(this->dtype->dtype) == INTEGER) ? get<int>(this->value) : get<double>(this->value);
                auto v2 = (*get<np<Primitive>>(that->dtype->dtype) == INTEGER) ? get<int>(that->value) : get<double>(that->value);
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(REAL)), v1 - v2);
            }
            return nullptr;
        }
        np<Literal> mul(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == INTEGER && *get<np<Primitive>>(that->dtype->dtype) == INTEGER) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(INTEGER)), get<int>(this->value) * get<int>(that->value));
            }
            if ((*get<np<Primitive>>(this->dtype->dtype) == INTEGER || *get<np<Primitive>>(this->dtype->dtype) == REAL) && 
            (*get<np<Primitive>>(that->dtype->dtype) == INTEGER || *get<np<Primitive>>(that->dtype->dtype) == REAL)) {
                auto v1 = (*get<np<Primitive>>(this->dtype->dtype) == INTEGER) ? get<int>(this->value) : get<double>(this->value);
                auto v2 = (*get<np<Primitive>>(that->dtype->dtype) == INTEGER) ? get<int>(that->value) : get<double>(that->value);
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(REAL)), v1 * v2);
            }
            return nullptr;
        }
        np<Literal> mod(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == INTEGER && *get<np<Primitive>>(that->dtype->dtype) == INTEGER) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(INTEGER)), get<int>(this->value) % get<int>(that->value));
            }
            return nullptr;
        }
        np<Literal> div(np<Literal> that) {
            if ((*get<np<Primitive>>(this->dtype->dtype) == INTEGER || *get<np<Primitive>>(this->dtype->dtype) == REAL) && 
            (*get<np<Primitive>>(that->dtype->dtype) == INTEGER || *get<np<Primitive>>(that->dtype->dtype) == REAL)) {
                auto v1 = (*get<np<Primitive>>(this->dtype->dtype) == INTEGER) ? get<int>(this->value) : get<double>(this->value);
                auto v2 = (*get<np<Primitive>>(that->dtype->dtype) == INTEGER) ? get<int>(that->value) : get<double>(that->value);
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(REAL)), v1 / v2);
            }
            return nullptr;
        }

        /*

            Boolean
            
        */
        np<Literal> andOp(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == BOOLEAN && *get<np<Primitive>>(that->dtype->dtype) == BOOLEAN) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(BOOLEAN)), get<bool>(this->value) && get<bool>(that->value));
            }
            return nullptr;
        }
        np<Literal> orOp(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == BOOLEAN && *get<np<Primitive>>(that->dtype->dtype) == BOOLEAN) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(BOOLEAN)), get<bool>(this->value) || get<bool>(that->value));
            }
            return nullptr;
        }
        np<Literal> xorOp(np<Literal> that) {
            if (*get<np<Primitive>>(this->dtype->dtype) == BOOLEAN && *get<np<Primitive>>(that->dtype->dtype) == BOOLEAN) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(BOOLEAN)), get<bool>(this->value) ^ get<bool>(that->value));
            }
            return nullptr;
        }
        np<Literal> notOp() {
            if (*get<np<Primitive>>(this->dtype->dtype) == BOOLEAN) {
                return make_shared<Literal>(make_shared<TypeNode>(make_shared<Primitive>(BOOLEAN)), !get<bool>(this->value));
            }
            return nullptr;
        }

        /*

            Compartive
            
        */
        np<Literal> lt(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 < v2)); 
        }
        np<Literal> leq(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 <= v2)); 
        }
        np<Literal> gt(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 > v2)); 
        }
        np<Literal> geq(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 >= v2)); 
        }
        np<Literal> eq(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 == v2)); 
        }
        np<Literal> neq(np<Literal> that) {
            auto v1_t = *get<np<Primitive>>(this->dtype->dtype);
            auto v1 = (v1_t == INTEGER) ? get<int>(this->value) : ((v1_t == REAL) ? get<double>(this->value) : get<bool>(this->value));

            auto v2_t = *get<np<Primitive>>(that->dtype->dtype);
            auto v2 = (v2_t == INTEGER) ? get<int>(that->value) : ((v2_t == REAL) ? get<double>(that->value) : get<bool>(that->value));

            auto type = make_shared<TypeNode>(make_shared<Primitive>(ast::BOOLEAN));
            return make_shared<Literal>(type, (v1 != v2)); 
        }


        friend ostream& operator<< (ostream& stream, const Literal& literal) {
            std::visit([&](auto&& arg){stream << arg;}, literal.value);
            return stream;
        }
    };

    struct Variable : Node {
        np<TypeNode> dtype;
        string name;
        np<Literal> value;

        Variable(np<TypeNode> dtype, string name, np<Literal> value) {
            this->dtype = dtype;
            this->name = name;
            this->value = value;
        }

        friend ostream& operator<< (ostream& stream, const Variable& var) {
            stream << *(var.dtype) << " " << var.name << " = " << *(var.value);
            return stream;
        }
    };

    struct ExpressionNode : Node {
        np<TypeNode> dtype;
        np<Literal> value;

        ExpressionNode(np<TypeNode> dtype, np<Literal> value) {
            this->dtype = dtype;
            this->value = value;
        }
    };

    struct Array : Node {
        int size;
        np<TypeNode> dtype;
        vector<np<Variable> > data;
        Array () { }
        Array(int size, np<TypeNode> dtype) {
            this->size = size;
            this->dtype = dtype;
            for(int i = 0; i < size; i++)
                data.push_back(make_shared<Variable>(dtype, "", make_shared<Literal>()));
        }

        friend ostream& operator<< (ostream& stream, const Array& arr) {
            stream << "array[" << arr.size << "] " << '\n';
            return stream;
        }
    };

    struct Record : Node {
        map<string, np<Variable> > variables;
        Record () { }
        Record(map<string, np<Variable> > variables){
            this->variables = variables;
        }

        friend ostream& operator<< (ostream& stream, const Record& rec) {
            stream << "record { ";
            for(auto u : rec.variables) stream << *(u.second) << "; ";
            stream << " }\n";
            return stream;
        }
    };
    
    struct Routine : Node {
        string name;
        map<string, np<Variable> > params;
        np<TypeNode> rtype;
        // vector<np<Statement> > stmts;
        
        Routine(string name, map<string, np<Variable> > params, np<TypeNode> rtype){
            this->name = name;
            this->params = params;
            this->rtype = rtype;
        }

        friend ostream& operator<< (ostream& stream, const Routine& routine) {
            stream << "routine " << routine.name << "( ";
            for(auto u : routine.params) stream << *(u.second) << ", ";
            stream << " )\n";
            return stream;
        }
    };

}
