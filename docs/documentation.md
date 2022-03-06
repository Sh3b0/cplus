# C+ Documentation

## Table of Contents

- [Documentation Guide](#Documentation-Guide)
- [Variable Declaration](#Variable-Declaration)
  - [Primitive types](#Primitive-types)
  - [User types](#User-types)

- [Output](#Output)
- [Comments](#Comments)

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



## Documentation Guide

The following rules are used in sections presenting language syntax:

- **Bold** text indicates a language literal (keyword or operator).

- <ins>Underlined</ins> text can be safely replaced with an equivalent syntax defined somewhere else in the document.

- *Italic* text is just for explanation and is not part of the syntax.

  

## Variable Declaration

### Primitive types:
- **integer**: supports integer numbers (8 bytes signed integers)
- **real**: supports real values (4 bytes)
- **boolean**: can only be true or false (1 byte)

### User types

- **array**: for grouping variables (“array elements”) of the same type together.
- **record**: for grouping logically related variables into a single construct.

**Syntax:**

- **var** <ins>Identifier</ins> : <ins>Type</ins> **is** <ins>Expression</ins>;
- **var** <ins>Identifier</ins> : <ins>Type</ins>;
- **var** <ins>Identifier</ins> **is** <ins>Expression</ins>;

<ins>Identifier</ins>

- Can be any string containing capital or small English letters, as well as digits (0-9) and underscore (_)
- Should not start with a digit.

<ins>Type</ins>:

- **integer**
- **real**
- **boolean**
- **record** **{** *Variable declarations separated by a semicolon* **}** **end**

- **array** <ins>Type</ins>
- **array** **[** <ins>Expression</ins> **]** <ins>Type</ins>
  - *Expression should be reducible to an integer representing array size*

**Semantics:**

- A place in memory is reserved for a variable of <ins>Type</ins> identified by <ins>Identifier</ins> and assigned an initial value of <ins>Expression</ins>
- <ins>Identifier</ins> is a string of capital or small English letters, digits, or the underscore (_), it cannot start with a digit.
- <ins>Expression</ins> and <ins>Type</ins> are discussed in the corresponding section of the document.
- If the initial value is specified in the declaration then the type can be omitted. In such a
  case, the type can be unambiguously deduced (“inferred”) from the expression that
  specifies the initial value.
- **Compound types such as arrays of records and multidimensional arrays are not supported.**
- Records containing an array/record field are however supported.

**Examples:**

```python
var a : integer is 20;
var b : boolean is false;
var c : real is 1.5;
var d : integer; # will not be initialized unless global
var x is 5;      # x becomes integer automatically
var y is true;   # y becomes boolean automatically
var z is 0.5;    # z becomes real automatically
```

### Type Declaration:

- **type** keyword allows introducing aliases for primitive or user data types.
- After a type declaration, the identifier following the keyword **type** is considered as a synonym for the type specified after the is keyword. The name introduced by such a declaration can be used everywhere in the program.
- Type declarations are only allowed in the global scope

**Examples:**

```python
type int is integer;  # int and integer can now be used interchangably
var x : int is 5;
```

````python
type Point2D is record { var x : integer; var y : integer } end;

var p : Point2D;
p.x := 1;
p.y := 2;
````

```python
var numbers : array[3] integer;

numbers[1] := 5;      # sets the first element of the array
var y is numbers[5];  # gets the last element of the array
```
**Notes:**

- Arrays in C+ are 1-indexed (First element is at index 1)



## Output

- The language has no input mechanism.
- The **print** and **println** keywords are used to evaluate and print expressions to `stdout`.

**Syntax:**

- **print** <ins>Expression</ins>**;**
- **print** "any string"**;**
- **println** <ins>Expression</ins>**;**
- **println** "any string"**;**

**Semantics:**

- **println** is the same as **print** but prints an empty line after it's output.
- Only primitive types (**integer**, **real**, **boolean**) are printable.
- Special characters and escapes inside strings have no special meaning.

**Example:**

**Code:**

```python
println true;

var y is 5;

print y;
print " ";
println y * 10;
print "\n"\t";
```

**Console:**

```
1
5 50
\n"\t
```



## Comments

You can use the **#** symbol to mark the rest of a line as a comment.

**Examples:**

```python
# This is a comment
var x : integer; # This is also a comment 
```



## Expressions
### Arithmetic:
**Operators:**

- **+**: Addition
- **-**: Subtraction
- *****: Multiplication
- **/**: Division
- **%**: Modulo

**Example:**

```python
var x is 5 + 5;  # 10
```
### Relational:
**Operators:** 

- **<**: Less than
- **<=**: Less than or equal
- **>**: Greater than
- **>=**: Greater than or equal 
- **=**: Equals
- **/=**: Not equal

**Examples:**

```python
var y is 3 > 1;  # true
var z is 4 = 5;  # false
```
### Brackets:
**Operators:**

- **(**
- **)**

**Example:**

```python
var x is 1 - 4 + 5;      # 2
var y is 1 - (4 + 5);    # -8
var z is 1 + 4 * 5;      # 21
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

```python
var x : integer;
var y : real;
var z : boolean;

x := 5.9;        # 5
x := true;       # 1
y := 1;          # 1.0
y := false;      # 0.0
z := 4;          # 1 (true)
z := 0.0;        # 0 (false)
```

## Conditionals

**Syntax:**

- **if** <ins>Expression</ins> **then** <ins>Body</ins>  **end**

- **if** <ins>Expression</ins> **then** <ins>Body</ins>  **else** <ins>Body</ins> **end**

**Semantics:**

- Executes <ins>Body</ins> if <ins>Expression</ins> evaluates to true.
- Integer or real expressions are compared with 0 and 0.0 respectively to get a boolean expression.

**Examples:**

```python
var x : integer is 1;
if x then 
    x := x * -1;      # x becomes -1
end
```
```python
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

- Keeps executing <ins>Body</ins> until <ins>Expression</ins> evaluate to false.
- Integer and real expressions are compared with 0 and 0.0 respectively to get a boolean expression.

**Examples:**

```python
var x is 10;

while x > 0 loop
    x := x - 1;
end

println x; # 0
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

```python
# Normal for loop

var x is 0;

for i in 0 .. 4 loop
    x := x + i;
end

# Values of i and x during execution:
# i     x
# 0     0
# 1     1
# 2     3
# 3     6
# 4     10
```
```python
# For loop with reverse
var x is 0;

for i in reverse 0 .. 4 loop
    x := x + i;
end

# Values of i and x during execution:
# i     x
# 4     4
# 3     7
# 2     9
# 1     10
# 0     10
```



## Routines

**Syntax:**

- **routine** <ins>Identifier</ins> **(** *parameter decelerations* **)** **is** <ins>Body</ins> **end**
  - *Parameter declarations have the form* <ins>Identifier</ins> : <ins>Type</ins> *and are separated by a comma*
  - *A routine can have no parameters*
- **routine** <ins>Identifier</ins> **(** *parameter decelerations* **)** **:** <ins>Type</ins> **is** <ins>Body</ins> **end**

**Semantics:**

- A routine with no return <ins>Type</ins> specified is a procedure, it can be called from other routines and must be terminated with **return;**

- A routine with a return <ins>Type</ins> specified is a function, it can be called from other routines and can appear in <ins>Expression</ins>s and must be terminated with **return** <ins>rval</ins>**;**

  - <ins>rval</ins> is a variable or literal of type <u>Type</u>

- Program starts execution from the **main** routine which must have the following signature:

  ```python
  routine main() : integer is
  	
  	# Variable declarations and statements
  	
  	return 0; # it may return other integer values as well.
  end
  ```

- **Recursive routines are not supported**



**Working Program Example:**

```python
type int is integer;
var PI is 3.14159;

routine say_hi() is
    println "Hello C+";
    return;
end

routine power(a: int, b: int) : int is
    var result is 1;
    for i in 0 .. b - 1 loop
        result := result * a;
    end
    return result;
end

routine circle_area(radius: real) : real is
	return PI * radius * radius;
end

routine main() : int is
	say_hi();
	var area is circle_area(5.0);
	println power(2, 4) + area;
	return 0;
end
```

**Output**

```
Hello!
94.539750
```

