# C+ Formal Grammar

## Guide

- `rule : defintion` defines a grammar production rule.
- `{ rule }` matches zero or more production rules.
- `[ rule ]` matches zero or one production rules
- `( var1 | var2 )` matches exactly one of the variants (there can by one or more options)
  - Parenthesis can be omitted in case of no ambiguity.

- `"literal"` matches the exact symbol or keyword.

- **Regexes used below**

  ```haskell
  IntegerLiteral:   ?[0-9]+
  RealLiteral:      -?[0-9]+\.[0-9]+
  BooleanLiteral:   true|false
  Identifier:       [a-zA-Z_][a-zA-Z0-9_]*
  String:           \".*\" 
  ```

## Grammar

```haskell
Program :
	{ SimpleDeclaration | RoutineDeclaration }
	
SimpleDeclaration :
	VariableDeclaration | TypeDeclaration
```

```haskell
VariableDeclaration :
	"var" Identifier ":" Type [ "is" Expression ] ";"
	| "var" Identifier "is" Expression ";"
```

```haskell
Expression : 
        IntegerLiteral
        | RealLiteral
        | BooleanLiteral
        | ModifiablePrimary
        | RoutineCall
        | "(" Expression ")"
        | ( "not" | "-" ) Expression
        | Expression ( "+" | "-" | "*" | "/" | "%" | "<" | "<=" | ">" | ">=" | "=" | "/=" | "and" | "or" | "xor" ) Expression
```

```haskell
ModifiablePrimary : Identifier | Identifier "." Identifier | Identifier "[" Expression "]"
RoutineCall : Identifier "(" [ Expression { "," Expression } ] ")"
```

```haskell
TypeDeclaration : "type" Identifier "is" Type ";"
Type : PrimitiveType | ArrayType | RecordType | Identifier

PrimitiveType : "integer" | "real" | "boolean"
ArrayType : "array" "[" Expression "]" Type

RecordType :
	"record" "{" VariableDeclaration { ";" VariableDeclaration } "}" "end"
```

```haskell
RoutineDeclaration :
	"routine" Identifier "(" Parameters ")" [ ":" Type ] "is" Body "end"
    
Parameters : ParameterDeclaration { "," ParameterDeclaration }
ParameterDeclaration : Identifier ":" Identifier
Body : { SimpleDeclaration | Statement }

Statement :
	Assignment | WhileLoop | ForLoop | IfStatement | ReturnStatement | PrintStatement | ( RoutineCall ";" )
```

```haskell
Assignment : ModifiablePrimary ":=" Expression ";"
WhileLoop : "while" Expression "loop" Body "end"
ForLoop : "for" Identifier "in" [ "reverse" ] Expression ".." Expression "loop" Body "end"
IfStatement : "if" Expression "then" Body [ "else" Body ] "end"
ReturnStatement : "return" [ Expression ] ";"
PrintStatement : ( "print" | "println" ) ( Expression | String ) ";"
```

