# Raven's specification
## 1. Introduction
Raven is a fast, pragmatic general-purpose language mixing imperative and object-oriented programming.

## 2. Basic concepts
This section covers most basic semantical rules of Raven.

### 2.1. Programs
Each Raven program consists of:
- One (and only one) `main` function.
- Zero or more user-defined functions with any name except `main`.

### 2.2. Values and types
Raven supports a strict static type system. Each variable, field or argument has a type assigned at compilation that can't change at any moment in runtime.

Each *literal* can represent one or more types. For example, `"Hello, World!"` can only be a `string`, while `56` can represent signed or unsigned integer types with any bit size (`int8, int16, int32, int64`, `uint8`, `uint16`, `uint32`, `uint64`, `float8`...).

These are all primitive types + some basic structural types in Raven:
- **String** (`string`): represents a string of characters. Can contain escaped characters, such as newlines (`\n`), tabs (`\t`), among others.
- **Numeric**: various types representing any numeric value, which can be a integer (`int`) or floating-point number (`float`). Integers can be *signed* (`int`) or *unsigned* (`uint`). All numeric types can have an optional bit-size: 8, 16, 32 or 64, with the default one being 32.
- **Boolean** (`boolean`): type consisting of only the `true` and `false` literals. An integer can be converted into a boolean (`1` = `true`, `0` = `false`) and viceversa.
- **Void** (`void`): represents **absence of a returned value in functions**. Not to be confused with `None` types of other languages, which represent **absence of value**.
- **Array** (`[T]`): represents a list of elements sharing a unique type.
- **Map** (`[K, V]`): represents a list of key-value pairs sharing unique key and value types.
> Note that the map's type expression `[K, V]` will most likely change to `{K, V}` in order to remove possible confussion with arrays and make it easier for implementation of different type expressions.
> This change also implies that map literals will change to `{"example": 3}` from `["example": 3]`.

Functions in Raven are first-class, which means that functions have their own type (`(A...)R`) and have a literal:
```
let add = function(a: int, b: int) int { return a + b; }; // Here "add" is "(int, int)int"
```

### 2.3. Declarations
In Raven, *declarations* are any statement that explicitly declares either a new usable member with a type and value (variables, functions) or a completely new type (structures).

#### 2.3.1. Variables
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

#### 2.3.2. Functions
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

#### 2.3.3. Structures
Structures represent a whole new structured type. They contain fields and methods, may or may not have a constructor and, opposite to other languages, can't inherit other structures (although composition, consisting of using structures inside structures, is allowed and even encouraged).

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
- It has 2 private fields: "x" and "y", both floats. They will be automatically assigned to what's passed when creating the vector, as the arguments of the constructor have the same name.
- It has two methods: "getX" and "getY", which return fields "x" and "y", as they aren't accessible from outside the structure.

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
As we passed `5.0` and `2.5` as arguments when creating the object, the fields "x" and "y" will have those values respectively. You may choose to include the name of the arguments when creating this object, although this is completely optional.
