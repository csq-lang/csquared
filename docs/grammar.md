# Full grammar
|**Usage**|**Notation**
:-----:|:-----:
definition |<code>**=**</code>
concatenation |<code>,</code>
termination |<code>;</code>
alternation |<code>&#124;</code>
optional |<code>[ ... ]</code>
repetition |<code>{ ... }</code>
grouping |<code>( ... )</code>
terminal string |<code>" ... "</code>
terminal string |<code>' ... '</code>
comment |<code>(* ... *)</code>
special sequence |<code>? ... ?</code>
exception |<code>-</code>

> Table from [this GitHub gist](https://gist.github.com/refs/e6e84057eb047ee5f306b76e13f76d1e).

```ebnf
Program = { Ignorable | Declaration | Statement } ;

Ignorable = WS | Comment ;

Comment = "//", { anychar - "\n" } ;

(* ===== Lexical ===== *)

boolean = "true" | "false" ;

ident = ( letter, { letter | digit | "_" } ) - reserved ;

number = digit, { digit }, [ ".", digit, { digit } ] ;

string =
      '"', { ( anychar - '"' ) | interpolated }, '"'
    | "'", { ( anychar - "'" ) | interpolated }, "'" ;

interpolated = "$", "{", { anychar - "}" }, "}" ;

tag = "@" string ; (* no whitespace between "@" and string, so that's why there is no comma *)

letter =
      "a" | "b" | "c" | "d" | "e" | "f" | "g"
    | "h" | "i" | "j" | "k" | "l" | "m" | "n"
    | "o" | "p" | "q" | "r" | "s" | "t" | "u"
    | "v" | "w" | "x" | "y" | "z"
    | "A" | "B" | "C" | "D" | "E" | "F" | "G"
    | "H" | "I" | "J" | "K" | "L" | "M" | "N"
    | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
    | "V" | "W" | "X" | "Y" | "Z" ;

digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

WS = " " | "\t" | "\n" | "\r" ;

anychar = ? any character ? ;

bit_depth = "8" | "16" | "32" | "64" ;

(* ===== Literals ===== *)

Literal =
      number
    | string
    | boolean
    | tag
    | ArrayLiteral
    | MapLiteral ;

ArrayLiteral = "[", [ Expr, { ",", Expr } ], "]" ;

KeyValue = "[", Expr, "]", ":", Expr ;

MapLiteral = "{", [ KeyValue, { ",", KeyValue } ], "}" ;

(* ===== Types ===== *)

Type =
    ident - "void"
    | ArrayType
    | MapType
    | FunctionType ;

ArrayType = "[", Type, "]" ;

MapType = "{", Type, ",", Type, "}" ;

FunctionType = "(", [ Type, { ",", Type } ], ")", Type ;

TypeAnnot = ":", Type ;

(* ===== Declarations ===== *)

Declaration =
      Variable
    | Function
    | Structure
    | Enumeration ;

Variable =
    ( "let" | "const" ),
    ident,
    [ TypeAnnot ],
    "=",
    Expr ;

Function =
    { Decorator },
    "function",
    ident,
    "(",
        [ Parameter, [ { ",", Parameter } ] ],
    ")",
    Type,
    "{", Block, "}" ;

Decorator = "#", ident ;

Parameter =
      "self"
    | "...", ident, TypeAnnot
    | ident, TypeAnnot, [ "=", Expr ] ;

StructParameter = ident, TypeAnnot, [ "=", Expr ] ;

Structure =
    "struct",
    ident,
    [ "(", StructParameter, { ",", StructParameter } , ")" ],
    "{",
        { StructureField | Function }, ";",
    "}" ;

StructureField =
    [ "private" ],
    ident,
    [ TypeAnnot ],
    "=",
    Expr ;

Enumeration =
    "enum",
    ident,
    "{",
        ident, { ",", ident },
    "}" ;

(* ===== Statements ===== *)

Statement =
      If
    | Switch
    | Defer
    | Return
    | Throw
    | Try
    | Loop
    | Declaration
    | Expr ;

If =
    "if", "(", Expr, ")", "{", Block, "}",
    { "else", "if", "(", Expr, ")", "{", Block, "}" },
    [ "else", "{", Block, "}" ] ;

Switch =
    "switch", "(", Expr, ")", "{",
        { Case },
        [ "default", "{", Block, "}" ],
    "}" ;

Case =
    "case",
    "(",
        Expr, { ",", Expr },
    ")",
    "{", Block, "}" ;

Defer =
      "defer", Expr
    | "defer", "{", Block, "}" ;

Return = "return", [ Expr ] ;

Throw = "throw", Expr ;

Try =
    "try", "{", Block, "}",
    "catch", "(", ident, ")", "{", Block, "}" ;

Loop =
      For
    | While
    | Until
    | Repeat ;

For =
    "for",
    "(",
        ident, { ",", ident },
        "in",
        Expr,
    ")",
    "{", Block, "}" ;

While =
    "while",
    [ "(", Expr, ")" ],
    "{", Block, "}" ;

Until =
    "until",
    "(", Expr, ")",
    "{", Block, "}" ;

Repeat =
    "repeat",
    Expr,
    "{", Block, "}" ;

Block = { Ignorable | Statement, ( ";" | "}" ) } ;

(* ===== Expressions ===== *)

Expr = Assignment ;

Assignment =
    LogicOr,
    [ ( "=" | "+=" | "-=" | "*=" | "/=" ), Assignment ] ;

LogicOr =
    LogicAnd, { "or", LogicAnd } ;

LogicAnd =
    Equality, { "and", Equality } ;

Equality =
    Relational, { ( "==" | "!=" ), Relational } ;

Relational =
    Additive, { ( "<" | "<=" | ">" | ">=" ), Additive } ;

Additive =
    Multiplicative, { ( "+" | "-" ), Multiplicative } ;

Multiplicative =
    Exponent, { ( "*" | "/" | "%" ), Exponent } ;

Exponent =
    Unary, [ "^", Exponent ] ;


Unary =
      ( "-" | "#" ), Unary
    | Range ;

Primary =
      Literal
    | ident
    | "(" Expr ")"
    | FunctionCall
    | AnonFunction
    | New
    | Spawn ;

Postfix =
    Primary,
    { "." ident
    | "[" Expr "]"
    },
    [ "++" | "--" ] ;

Range =
    Postfix, "..", Postfix ;

(* Reserved keywords *)
function = "function" ;
if = "if" ;
else = "else" ;
switch = "switch" ;
case = "case" ;
default = "default" ;
while = "while" ;
for = "for" ;
return = "return" ;
throw = "throw" ;
struct = "struct" ;
enum = "enum" ;
const = "const" ;
let = "let" ;
in = "in" ;
import = "import" ;
new = "new" ;
repeat = "repeat" ;
until = "until" ;
defer = "defer" ;
try = "try" ;
catch = "catch" ;
spawn = "spawn" ;
private = "private" ;
self = "self" ;
or = "or" ;
and = "and" ;
reserved =  
        if | else | switch | case | default | while | for 
        | return  | throw | struct | enum | const | let 
        | in | import | new | repeat | until | defer | try 
        | catch | spawn | private | self | or | and ;
```
