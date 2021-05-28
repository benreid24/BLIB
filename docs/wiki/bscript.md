# bScript
**bScript** is the built in scripting language included in BLIB. A generic, reusable shift-reduce parser is provided as well for other parsing needs. The
general syntax for **bScript** is documented below. Specific function libraries are added by user supplied derived classes of `Script`. **TODO: Add link
to the example of this**

## Design Rationale

**bScript** derives much of its design from Python and C. As such, much of its syntax will be familiar to someone versed in either language. **bScript**
aims to be lightweight and concise while still allowing for fairly complex systems to be built using it, within reason. The majority of its use will be
within games to script events and AI, however, which limits the need for a completely robust language.

***

# Syntax

## Valid Identifiers

Valid identifiers in **bScript** begin with an underscore or a letter which is then followed by any alphanumeric sequence and or underscores.

### Good Examples

- `_123`
- `_abd_def`
- `CamelCase`
- `snake_case`
- `SpongeBob_Case`
- `camel1_case2`

### Bad Examples

- `1a`: Cannot start with a number
- `not-good`: Cannot contain symbols other than underscore
- `uh oh`: Cannot have whitespace

## Built-in Types

The following types are part of the core language:

| Type      | Description                           | Example                    | Truthiness                           |
|-----------|---------------------------------------|----------------------------|--------------------------------------|
| Void      | No type                               | N/A                        | `false`                              |
| Bool      | True or false                         | `true` or `false`          | N/A                                  |
| Numeric   | Floating point number                 | `5` or `-34.56`            | Nonzero is `true`, zero is `false`   |
| String    | String of characters                  | `"hello world"`            | Nonempty is `true`, empty is `false` |
| Array     | Resizable list of values              | `[]` or `[1, true, "str"]` | Nonempty is `true`, empty is `false` |
| Function  | Callable function                     | `def fn() {return 5; }`    | `false`                              |
| Reference | Modifiable reference to another value | `&someVar`                 | Referenced Value                     |


## Built-in Operations

The following operations are supported natively by **bScript**. References to valid operand types are always valid in expressions as they are dereferenced before any operations.

### Operations

| Operation              | Operator | Left Type(s) | Right Type(s)    | Result Type | Example                               | Result                        |
|------------------------|----------|--------------|------------------|-------------|---------------------------------------|-------------------------------|
| Numeric Addition       | +        | Numeric      | Numeric          | Numeric     | `5 + 45.5`                            | `50.5`                        |
| String Concat          | +        | String       | String, Numeric  | String      | `"hello " + "world"` or `"fish" + 10` | `"hello world"` or `"fish10"` |
| Array Append           | +        | Array        | Any              | Array       | `["my", "list"] + 5`                  | `["my", "list", 5]`           |
| Subtraction            | -        | Numeric      | Numeric          | Numeric     | `67 - 9000`                           | `-8933`                       |
| Negative               | -        | N/A          | Numeric          | Numeric     | `-56`                                 | `-56`                         |
| Numeric Multiplication | *        | Numeric      | Numeric          | Numeric     | `36.5 * 92`                           | `3358`                        |
| String Multiplication  | *        | String       | Numeric(integer) | String      | `"fish" * 3`                          | `"fishfishfish"`              |
| Array Multiplication   | *        | Array        | Numeric(integer) | Array       | `["fish"] * 3`                        | `["fish", "fish", "fish"]`    |
| Division               | /        | Numeric      | Numeric          | Numeric     | `100 / 10`                            | `10`                          |
| Exponent               | ^        | Numeric      | Numeric          | Numeric     | `10^2`                                | `100`                         |

### Comparators

Note that strings are compared alphabetically for less/greater comparisons. Arrays are compared by length. For Boolean `true` is greater than
`false`. Numeric values are compared numerically.

| Comparison    | Operator | Type(s)                  |
|---------------|----------|--------------------------|
| Equality      | ==       | Any                      |
| Not Equal     | !=       | Any                      |
| Greater Than  | >        | Any (excluding Function) |
| Greater Equal | >=       | Any (excluding Function) |
| Less Than     | <        | Any (excluding Function) |
| Less Equal    | <=       | Any (excluding Function) |

### Boolean Logic

Used to combine boolean values.

| Operation | Operator |
|-----------|----------|
| And       | `and`    |
| Or        | `or`     |
| Not       | `not`    |

### Order of Operations

| Evaluation Order | Operation(s)             |
|------------------|--------------------------|
| 1                | Parenthesis              |
| 2                | Negative                 |
| 3                | Exponent                 |
| 4                | Multiplication, Division |
| 5                | Addition, Subtraction    |
| 6                | Comparisons              |
| 7                | Boolean Not              |
| 8                | Boolean And              |
| 9                | Boolean Or               |

***

## Variables and Expressions

Variables may be created or modified using the following assignment statement syntax: `var = <expression>;` where `<expression>` can be just about
anything, including functions, function calls, other variables, array access, literal values, or a combination of all of the above combined with valid
operations.

### Literals

Raw values of various types may be hardcoded as literals. See the above types table for examples.

### Arrays

Arrays may be constructed and used as follows:
```
arr1 = [1,2,3];

arr2 = [];
arr2.append(5);        // now [5]
arr2.insert(0, 3, 4);  // now [3, 4, 5]
arr2.erase(2);         // now [3, 4]
var = arr2.length;     // var = 2

arr2[0] = arr1[1];     // arr2 = [2, 4]
```

Arrays have the following built-in methods and properties:
| Name   | Type     | Parameters                          | Description                                                                           |
|--------|----------|-------------------------------------|---------------------------------------------------------------------------------------|
| length | Numeric  | N/A                                 | Number of items in the array                                                          |
| clear  | Function | N/A                                 | Clears all items in the array                                                         |
| append | Function | value: Any                          | Appends the value to the end of the array                                             |
| resize | Function | size: Numeric, fill: Any (optional) | Sets the new size of the array with an optional fill value. Fill is defaulted to void |
| insert | Function | index: Numeric, value(s): Any       | Inserts at the given index the list of values provided                                |
| erase  | Function | index: Numeric                      | Erases the element at the given index                                                 |

### Properties

All variables may be assigned nested properties no matter the type of the variable. These function similar to class members. Properties may be used
as follows:
```
var = "hello world";
var.nestedProp1 = 5;
var.nestedProp2 = false;

anotherOne = var.nestedProp1;   // anotherOne = 5
myBool = var.at("nestedProp2"); // myBool = false
keys = var.keys();              // keys = ["nestedProp1", "nestedProp2"]
```

The following functions are provided on all variables for interacting with their properties:
| Name | Parameters  | Return                                                               |
|------|-------------|----------------------------------------------------------------------|
| get  | key: String | Property with the given name or void if does not exist               |
| keys | N/A         | Array of strings of all property names. Excludes built-in properties |

Properties may be named with any valid identifier except for the list of reserved built-in names:
| clear  |
|--------|
| append |
| insert |
| erase  |
| length |
| keys   |
| at     |

### References

Similar to C++ references, **bScript** references are pointers to other values but do not require any special dereference syntax to access the underlying
value. References may be used anywhere where their underlying value can be used.

```
var = 55;
ref = &var;
ref = 100;  // now var = 100

def mod_function(ref) {
    ref = "new value";
}
change_me = "i will be changed";
mod_function(&change_me);
// now change_me = "new value"
```

### Functions

Functions are first class objects in **bScript** and may be passed around like any other value. This allows for useful features like dynamic callbacks to
be used by even simple scripts. Functions may be defined and used as follows:

```
def my_function(value, value_cb) {
    return value + value_cb();
}

def my_value_cb() {
    return 5;
}

alias = my_function;
var = alias(5, my_value_cb); // var = 10
```

## Statements

As with any other language, **bScript** is evaluated as a series of statements. Beyond function calls and definitions, and variable assignment as listed
above, the following constructs are available to implement logic.

### Conditional Statements

Works like any other language:
```
if (var == 5) {
    doSomething();
}
elif (var < 0) {
    doSomethingElse();
}
else {
    return someErrorValue;
}
```

### While Loops

Very familiar to any programmer:
```
i = 0;
my_list = [];
while (i <= 5) {
    my_list.append(i);
    i = i + 1;
}
// my_list = [1, 2, 3, 4, 5]
```

### For Loops

Python inspired container iteration:
```
my_list = [1, 2, 3, 4];
sum = 0;
for (n in my_list) {
    sum = sum + n;
}
// sum = 10
```