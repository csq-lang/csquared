# Raven's specification
# 1. Introduction
Raven is a fast, pragmatic general-purpose language mixing imperative and object-oriented programming.

> You are reading a very early version of the specification, which means there might be some specific details missing, information might be vage/poor, etc. This document will become more and more formal and informative with time, but for now, don't expect much, as not even the prototype compiler is done. \
>\
> If you notice any errors or have suggestions, please open an issue or a pull request.

# 2. Semantics
> **DISCLAIMER:** work in progress!

This section covers most semantical rules inside of Raven.

## 2.1. Programs
Each Raven program consists of:
- One (and only one) `main` function.
- Zero or more user-defined functions with any name except `main`.

## 2.2. Values and types
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

Another feature functions implement is the variable argument. Each function can have at least one argument which has an undefined length. The variable argument is syntax sugar which, when compiled, simply converts all passed values into a single array.

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

## 2.4. Statements
> **DISCLAIMER:** work in progress!

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

### Structure declaration
Declares a new structure, with a variable number of fields and methods. It may contain construction fields, although optional. See [2.3.3.](<spec#2.3.3. Structures>) for more details.

**Examples**:
```
struct Point { x: int, y: int }; // Has two fields: "x" and "y", both integers.
struct Person { name: string, age: int } // Has two fields of different types.
struct Empty {}; // Empty structure. Completely valid.

// Structure which only contains one method: "greet".
struct Greeter {
  function greet(name: string) {
    print("Hello, ${name}!");
  }
} 

// Structure with constructor, two fields (one of them constant) and one method.
struct Dog(name: string) {
  name: string,
  const scientific_name: "Canis familiaris",

  function bark(self) {
    print("Woof! Woof! My name is ${self.name}! Woof!");
  }
}
```
