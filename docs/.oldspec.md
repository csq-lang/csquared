# Raven's specification
# 1. Introduction
Raven is a fast, pragmatic general-purpose language mixing imperative and object-oriented programming.

> You are reading a very early version of the specification, which means there might be some specific details missing, information might be vage/poor, etc. This document will become more and more formal and informative with time, but for now, don't expect much, as not even the prototype compiler is done. \
>\
> If you notice any errors or have suggestions, please open an issue or a pull request.\
>\
>Also note that I am natively a Spanish speaker and don't have perfect, native english. It's a struggle to make even a paragraph a formal and well-structured piece you expect to find in a specification. As I said, feel free to correct anything in a pull request! I may also write a version of the specification in Spanish and translate every version to English to make the process easier, although the English specification will get updates later.

# 2. Semantics
> **DISCLAIMER:** work in progress!

This section covers most semantical rules inside of Raven.

*Note:* two slashes (`//`) mean a comment, which does not affect the program in any way and allows to give or get information about the program. They will be used frequently through this specification.

## 2.1. Programs
Each Raven program consists of:
- One (and only one) `main` function.
- Zero or more user-defined functions with any name except `main`.

## 2.2. Values and types
> This section *might* need further extension.

Raven supports a strict static type system. Each variable, field or argument has a type assigned at compilation that can't change at any moment in runtime.

Each *literal* can represent one or more types. For example, `"Hello, World!"` can only be a `string`, while `56` can represent signed or unsigned integer types with any bit size (`int8, int16, int32, int64`, `uint8`, `uint16`, `uint32`, `uint64`, `float8`...).

These are all primitive types + some basic structural types in Raven:
- **String** (`string`): represents a string of characters.
- **Numeric**: various types representing any numeric value, which can be a integer (`int`) or floating-point number (`float`). Integers can be *signed* (`int`) or *unsigned* (`uint`). All numeric types can have an optional bit-size: 8, 16, 32 or 64, with the default one being 32.
- **Boolean** (`boolean`): type consisting of only the `true` and `false` literals. An integer can be converted into a boolean (`1` = `true`, `0` = `false`) and viceversa.
- **Void** (`void`): represents **absence of a returned value in functions**. Not to be confused with `None` types of other languages, which represent **absence of value**.
- **Array** (`[T]`): represents a list of elements sharing a type.
- **Map** (`{K, V}`): represents a list of key-value pairs sharing key and value types.

Functions in Raven are first-class, which means that functions have their own type (`(A...)R`) and have a literal:
```
let add = function(a: int, b: int) int { return a + b; }; // Here "add" is "(int, int)int"
```

String literals can contain:
- Escaped characters, such as newlines (`\n`), tabs (`\t`), etc.
- Variables/values/calls using string interpolation. For example, if we want to introduce the [variable](<spec#2.3.1. Variables>) "five" of value `5` into a string saying "The number is <five>", we interpolate it by writing `The number is ${five}` (`${five}` being the interpolation), which compiles to `The number is 5`.

## 2.3. Declarations
In Raven, *declarations* are any statement that explicitly declares either a new usable member with a type and value (variables, functions) or a completely new type (structures).

### 2.3.1. Variables
Variables are identifiers with an assigned type and value. They can be mutable (their value can vary) or immutable (their value is only the initially assigned one).

To declare a mutable variable, the `let` keyword is used:
```
let you = "cool"; // Let "you" be a string with variable value "cool"
you = "even cooler";
```

To declare an immutable variable, the `const` keyword is used:
```
const you = "cool"; // Let "you" be a string with the constant value "cool"
you = "even cooler"; // ERROR: "you" is constant.
```

All variables have a scope. The scope determines who can access the variable:
```
const x = 1; // accessible in all the program

function main() int {
  const y = 2; // accessible inside the main function
  (function() {
    const z = 3; // accessible only inside this anonymous function
  })();
  return 0;
}
```

Variable shadowing (redeclaring a variable in a lower scope) is illegal in Raven. If we, for example, redeclared "x" in the main function, we would get a compile error.

### 2.3.2. Functions
Functions, in Ravn, are values that when called run a chain of statements (=block). Functions have:
- Various arguments with a name and type.
- One optional return value.

To declare a function, the `function` keyword is used:
```
// Declare function add, which has integer arguments "a", "b" and returns an integer.
function add(a: int, b: int) int {
  return a + b;
}
```
If the function returns no value, assign it's return type to `void`.

You can't declare the exact same function two times (same name, arguments and return type), but you can have various overloads if you change the type of arguments.

One clear example of having various overloads is the `print` function, which can print any primitive type as a string. The way this works is by having an overload for each type.

Another feature functions implement is the variable argument. Each function can have at least one argument which has an undefined length. The variable argument is syntax sugar which, when compiled, simply converts all values passed to the variable argument into a single array.

The `add_all.rvn` example shows these two features:
```
function addAll(...numbers: int) int {
  let sum = 0;
  for (number in numbers) {
    sum += number;
  }
  return sum;
}

function addAll(...numbers: float) float {
  let sum = 0;
  for (number in numbers) {
    sum += number;
  }
  return sum;
}

function main() {
  print(addAll(3, 2, 6, 10)); // 21
  print(addAll(3.02, 2.06, 6.1, 10.0)); // 21.9

  let a, b, c: uint = 5, 3, 7;
  print(addAll(a, b, c)) // ERROR: No overload for unsigned integers.
}
```

Functions can also have default arguments, which, unless provided by the user, have a default value.
```
function oldEnoughToDrink(age: int, minAge: int = 18) boolean {
  return age >= minAge;
}

function main() int {
  print(oldEnoughToDrink(19)); // true
  print(oldEnoughToDrink(19, 21)); // false
  return 0;
}
```
*`default.rvn` example*

### 2.3.3. Structures
Structures represent a whole new structured type. They contain fields and methods, may or may not have a constructor and, opposite to other languages, can't inherit other structures (although composition, consisting of using structures inside structures, is allowed and even encouraged). It is important to know that structures don't require fields or methods, meaning that an empty structure is completely valid, although useless.

To declare a structure, the `struct` keyword is used:
```
struct Vector(x: float, y: float) {
  private x: float,
  private y: float,

  function getX(self) float {
    return self.x;
  };

  function getY(self) float {
    return self.y;
  };
}
```

Let's analyze this structure:
- It has 2 arguments in it's constructor: "x" and "y", both floats.
- It has 2 private fields: "x" and "y", both floats.
- It has two methods: "getX" and "getY", which return fields "x" and "y", as they aren't accessible from outside the structure.

The constructor is used to assign values to fields at object creation. In this case, arguments "x" and "y", which then determine the value of fields of same name.
> By the time of writing this, constructors only serve this purpose, but this might change and allow for code execution at object creation and not only value passing.\\
> Another change that will most likely happen is the introduction of explicit syntax to assign a field inside of the structure to one of the arguments in the constructor.

Fields represent a data member of any type (primitive, structural or user-defined) accessible through and only through the structure. Fields can be either public or private, and every field is public until specified otherwise by using the `private` keyword. Private fields can't be accessed from outside the structure, only from the inside (for example, inside methods). Fields can also be constant, which means that the data inside them can't vary, similar to [immutable variables](<spec#2.3.1. Variables>).

To create an object from a structure, we use the `new` keyword:
```
struct Vector {
  // ...
}

function main() int {
  let vec = new Vector(5.0, 2.5);
  return 0;
}
```
The object contains:
- All declared fields with a respective value (as there is no `null`, every field **must** have a value).
- All declared methods as functions (which some might count as fields, considering that functions are first-class, although functions inside structures may always be called "methods").

To access a field inside a structure we use the member expression: `<object>.<member>`. The fields "x" and "y" are private, which means that we can't use them (`vec.x`/`vec.y`):
```
function main() int {
  let vec = new Vector(5.0, 2.5);
  print(vec.getX()); // Outputs "5.0"
  // print(vec.x); // ERROR: Field "x" is private.
  return 0;
}
```

As we passed `5.0` and `2.5` as arguments when creating the object, the fields "x" and "y" will have those values respectively. You may choose to include the name of the arguments when creating this object, although this is completely optional.

### 2.3.4. Enumerations
Enumerations allow to define a list of constant identifiers assigned each to a number. Variables can then be assigned to one of these identifiers.

Enumerations are useful for when you want a list of possible values a variable can have where each one has a constant name, instead of just being a number or string, avoiding comments or documents specifying that 0 is Idle, 1 is Active and 2 is Error and directly assigning a variable to Idle, Active or Error.

To declare enumerations, use the `enum` keyword:
```
enum State {
  Idle, // 0
  Active, // 1
  Error // 2
}
```

To then use enumerations, you can simply access each member exactly as in [structures](<spec#2.3.3. Structures>).
```
let state = State.Idle;  
```
One can also simply write `.<member>` if the variable/field/argument has the enumeration already annotated as it's type.
```
let state: State = .Idle;

struct Program(state: State = .Idle) {
  state: State,
}
```

## 2.4. Statements
A statement is code that solely causes a side effect (a new variable, repetition of an action through a loop...) and has no return value.

### Variable declaration
Declares a new variable, mutable (declared with `let`) or immutable (declared with `const`) (see [2.3.1.](<spec#2.3.1. Variables>)).

Declarations can optionally have a type annotation, type which, if not present, will be inferred. If the compiler can't infer the type, then a type annotation is mandatory If the compiler can't infer the type, then a type annotation is mandatory.

**Examples:**
```
let five = 5; // Mutable, inferred type (integer)
const name = "Raven"; // Immutable, inferred type (string)
let red, green, blue: uint8 = 255, 0, 255; // All mutable, specified type (uint8)

// ERRORS
let empty = []; // ERROR: Can't infer type.
let number: string = 7; // ERROR: Expression (integer) does not match type (string)
```

### Function declaration
Declares a new function with a static number of arguments and one return type.

See [2.3.2.](<spec#2.3.2. Functions>) for more details.

### Structure declaration
Declares a new structure, with a static number of fields and methods. It may contain construction fields, although optional.

See [2.3.3.](<spec#2.3.3. Structures>) for more details.

### Enumeration declaration
Declares a new enumeration with a static number of members. Each member consists only of an identifier.

See [2.3.4.](<spec#2.3.4. Enumerations>) for more details.

### If
Executes code only if a condition is true.
```
if (<condition>) {
  ...
}
```

If-statements can additionally include one or more else-if segment(s), which will check for another condition if the first one turns out false, and only one else segment, which runs if every condition was false.
```
if (<condition>) {
  ...
} else if (<condition>) {
  ...
} else { // All were false
  ...
}
```

There can be as many else-ifs as one desires, but it's encouraged to not chain a lot of them.
```
if (...) {
  ...
} else if (...) {
  ...
} else if (...) { 
  ...
} else if (...) {
  ...
}
```

### Switch
The switch statement allows to check for multiple possible values one expression can have.

An expression is supplied as the value we "switch on", then we define various cases, each one including one or more expressions/ranges. If one of these cases is true, the respective code is executed. The switch statement also allows to define a "default" case, which will execute if all other cases were false.
```
let x: uint = 5 + 4;
switch (x) {
  case (9) {
    print("Exactly 9!")
  },
  case (0..8) {
    print("Lower than 9!")
  },
  default {
    print("Over 9!")
  }
}
```


### Loops
There are 4 types of loops in Raven:
- For loops
- While loops
- Until loops
- Repeat loops

#### For loop
Loop over every element in a sequence, including:
- Every member of an array.
- Every pair of a map.
- Every character of a string.
- Every number in a range.
Among others.

**Some examples:**

Iterate over each element in an array (one of the most common uses).
```
let array = [5, 4, 78, 100, 2, 9 + 10, 21];

for (index, value in array) {
  print("Index: ${index}");
  print("Value: ${value}");
  print("---")
}
```
*Output:*
```
Index: 0
Value: 5
---
Index: 1
Value: 4
---
Index: 2
Value: 78
...
```

Iterate over each number in a range of 1 through 10.
```
for (index in 0..10) {
  print(" ${index}", ""); // no newline
}
```
*Output:*
```
0 1 2 3 4 5 6 7 8 9 10
```

#### While loop
Loop **while** a condition is true.
```
let i = 0;
while (i * 2 != 12) {
  print(i);
  i++;
}
```
*Output:*
```
0
1
2
3
4
5
6
```

#### Until loop
Loop **until** a condition is true (opposite of while loop).
```
let i = 0;
until (i * 2 == 12) {
  print(i);
  i++;
}
```
*Output:*
```
0
1
2
3
4
5
6
```

#### Repeat loop
Loops a specific amount of times.
```
let i = 0;
repeat 6 {
  print(i);
  i++;
}
```
*Output:*
```
0
1
2
3
4
5
6
```

### Defer
Defer is a very simple statement which will just delay the execution of a line/block to the end of the parent block. Defers are evaluated in opposite order.
```
function main() int {
  defer print("I already won!")
  defer print("See you at the end!")

  return 0;
}
```
*Output:*
```
See you at the end!
I already won!
```

> Defers are extremely useful for cleaner blocks, as all boilerplate can be written at the start, so all that's next is the actual code and not mandatory boilerplate.

### Return
Returns an expression, marking the end of the block. Return can only be used inside functions, as the file can't return a value.
> Raven is not [Lua](<https://lua.org>).

If return is used in a void function, a compile error is thrown. If code is written after return, a compile error is thrown.

### Throw 
Throw will stop execution of code (unless ran inside of a [try-catch statement](spec#Try-catch)) and throw an error.

Any expression can be passed to throw, which will convert it into a string and use it as the error message.
> I see this as too unsafe and limiting: what if the user tries to pass a structure as an error thinking it may work? What if the user wants to define a structure to use as their own "error" type, but can't directly pass the error and has to convert it into a string?\\
> Have to reconsider.

### Try-catch
Try-catch will safely run a block, and allow the user to access the error and handle it in the desired way (simply ignoring it, printing it, using a user-defined error handler, etc.).

# 3. Syntax

> **You can find the grammar [here](./grammar.md)**
