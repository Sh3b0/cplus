# C+ Documentation

## Table of Contents

- [Documentation Guide](#Documentation-Guide)
- [Variables](#Variables)
  - [Primitive types](#Primitive-types)
  - [Compound types](#Compound-types)
- [Expressions](#Expressions)
  - [Arithmetic](#Arithmetic)
  - [Relational](#Relational)
  - [Brackets](#Brackets)
  - [Assignment](#Assignment)
- [Conditionals](#Conditionals)
- [Loops](#Loops)
  - [While loop](#While-loop)
  - [For loop](#For-loop)
- [Routines](#Routines)
- [Output](#Output)
- [Comments](#Comments)



## Documentation Guide

The following rules are used in sections presenting language syntax:

- **Bold** text indicates a language literal (keyword or operator).

- <ins>Underlined</ins> text can be safely replaced with an equivalent syntax defined somewhere else in the document.

- *Italic* text between is just for explanation and is not part of the syntax.

  

## Variables

### Primitive types:
- **integer** : Supports integer numbers from -2,147,483,648 to 2,147,483,647 (4 bytes)
- **real** : Supports real values that fit in 8 bytes
- **boolean** : Only has 2 values true or false (1 byte)

**Syntax:**

- **var** <ins>Identifier</ins> : <ins>Type</ins> **is** <ins>Expression</ins>;

- **var** <ins>Identifier</ins> : <ins>Type</ins>;

- **var** <ins>Identifier</ins> **is** <ins>Expression</ins>;

**Semantics:**

- A place in memory is reserved for a variable of <ins>Type</ins> identified by <ins>Identifier</ins> and assigned an initial value of <ins>Expression</ins>
- <ins>Identifier</ins> is a string of capital or small English letters, digits, or the underscore (_), it cannot start with a digit.
- <ins>Expression</ins> and <ins>Type</ins> are discussed in the corresponding section of the document.
- If the initial value is specified in the declaration then the type can be omitted. In such a
  case, the type can be unambiguously deduced (“inferred”) from the expression that
  specifies the initial value.

**Examples:**

```
var a : integer is 20;
var b : boolean is false;
var c : real is 1.5;
var d : integer;
var x is 5;      # x becomes integer automatically
var y is true;   # y becomes boolean automatically
var z is 0.5;    # z becomes real automatically
```

### Compound Types:

**type** keyword allows the declaration of compound data types, as well as aliases for already existing primitive data types. After a type declaration, the identifier following the keyword **type** is considered as a synonym for the type specified after the is keyword. The name introduced by such a declaration can be used everywhere in its scope.

**Compound Types:**

- Array: for grouping variables (“array elements”) of the same type together.
- Record: for grouping logically related variables into a single construct.

**Syntax:**

- **type** <ins>Identifier</ins> **is** <ins>Type</ins>;

- <ins>Type</ins>:

  - **integer**
  - **real**
  - **boolean**
  - **record** **{** *Variable declarations separated by a semicolon* **}** **end**

  - **array** <ins>Type</ins>
  - **array** **[** <ins>Expression</ins> **]** <ins>Type</ins>
    - *Expression should be reducible to an integer literal representing array size*

**Examples:**

```
type int is integer;  # int and integer can now be used interchangably
var x : int is 5;
```

````
type Point2D is record {var x : integer; var y : integer} end;

var p : Point2D;
p.x := 1;
p.y := 2;
````

```
var numbers : array [3] integer;

numbers[1] := 5;      # sets the first element of the array
var y is numbers[5];  # gets the last element of the array
```
**Notes:**

- Arrays in C+ are 1-indexed (First element is at index 1)

- Array size can be omitted in the array type declaration. Sizeless declaration can be
  used only for specifying subroutine parameters.

  

## Expressions
### Arithmetic:
**Operators:**

- **+** : Addition
- **-** : Subtraction
- ***** : Multiplication
- **/** : Division

**Example:**

```
var x is 5 + 5;  # 10
```
### Relational:
**Operators:** 

- **<** : Smaller than
- **<=** : Smaller than or equal
- **>** : Bigger than
- **>=** : Bigger than or equal 
- **=** : Equals
- **/=** : Not equal

**Examples:**

```
var y is 3 > 1;  # true
var z is 4 = 5;  # false
```
### Brackets:
**Operators:**

- **(**
- **)**

**Example:**

```
var x is 1-4+5;      # 2
var y is 1-(4+5);    # -8
```

### Assignment:
**Operators:**

- **:=**

**Syntax:**

- <ins>ModifiablePrimary</ins> **:=** <ins>Expression</ins>
- <ins>ModifiablePrimary</ins>
  - <ins>Identifier</ins>									   *variable name*
  - <ins>Identifier</ins> **.** <ins>Identifier</ins>                   *record member access*
  - <ins>Identifier</ins> [ <ins>Expression</ins> ]             *array element access* 

**Example:**

```
var x : integer;
x := 5;
```

Note: assignments follow them following assignment table

![Assingment table](Assignment_table.png)



## Conditionals

**Syntax:**

- **if** <ins>Expression</ins> **then** <ins>Body</ins>  **end**

- **if** <ins>Expression</ins> **then** <ins>Body</ins>  **else** <ins>Body</ins> **end**

**Semantics:**

- Executes <ins>Body</ins> if <ins>Expression</ins> evaluates to true. [see assignment table for more info.](#Assignment)

**Examples:**

```
if x < 0 then 
    x := x * -1;
end
```
```
var isNegative : boolean;

if x < 0 then
    isNegative := true;
else
    isNegative := false;
end
```



## Loops

### While loop:
**Syntax:**

- **while** <ins>Expression</ins> **loop** <ins>Body</ins> **end**

**Semantics:**

- Keeps executing <ins>Body</ins> until <ins>Expression</ins> evaluate to false. [see assignment table for more info.](#Assignment)

**Examples:**

```
var x is 10;

while x > 0 loop
    x := x-1;
end

# x will decrease until it is equal to 0 where the while loop will exit
```
### For loop:
**Syntax:**

- **for** <ins>Identifier</ins> **in** <ins>Expression</ins> **..** <ins>Expression</ins> **loop** <ins>Body</ins> **end**

- **for** <ins>Identifier</ins> **in** **reverse** <ins>Expression</ins> **..** <ins>Expression</ins> **loop** body **end**

**Semantics:**

- Creates a variable of type inferred from <ins>Expression</ins> and identified by <ins>Identifier</ins>
- Initial value for variable is set to the first <ins>Expression</ins> (default)
- Keeps running <ins>Body</ins> as long as the variable is between the two <ins>Expression</ins>s
- After each iteration, the variable gets incremented by 1 (default)
- If **reverse** keyword is used:
  - Initial value of the variable is set to the second <ins>Expression</ins>
  - The variable is decremented by 1 after each iteration.

**Examples:**

```
# Normal for loop

var x is 0;

for i in 0 .. 4 loop
    x := x + i;
end

# Values of i and x went as follows:
# i     x
# 0     0
# 1     1
# 2     3
# 3     6
# 4     10
```
```
# For loop with reverse
var x is 0;

for i in reverse 0 .. 4 loop
    x := x + i;
end

# Values of i and x went as follows:
# i     x
# 4     4
# 3     7
# 2     9
# 1     10
# 0     10
```



## Routines

**Syntax:**

- **routine** <ins>Identifier</ins> **(** **)** **is** <ins>Body</ins> **end**

- **routine** <ins>Identifier</ins> **(** *parameter decelerations* **)** **is** <ins>Body</ins> **end**
  - *Parameter declarations have the form* <ins>Identifier</ins> : <ins>Type</ins> *and are separated by a comma*

- **routine** <ins>Identifier</ins> **(** **)** **:** <ins>Type</ins> **is** <ins>Body</ins> **end**

**Semantics:**

- If the type after the parameters section is specified then the function returns a value; therefore, its call can present in expressions. In that case, the value to be returned by the routine should specified in the routine body by the **return** keyword.

**Example:**

```
routine power (a : integer , b : integer) : integer is
    var result is 1;
    for i in 0 .. (b-1) loop
        result := result * a;
    end
    
    return result;
end

var y is power(2, 4);    # value of y is 16
```



## Output

The print keyword is used to simplify an expression and prints it to `stdout`.

**Syntax:**

- **print** <ins>Expression</ins>

**Example:**

**Code:**

```
print true;

var y is 5;
print y;

print y * 10;
```
**Console:**

```
true
5
50
```



## Comments

You can use the '#' to mark the rest of a line as a comment.

**Examples:**

```
# This is a comment
var x : integer; # This is also a comment 
```

