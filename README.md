# liteexpr

A light, expression language.


## Features

* Pass variables to be evaluated within the expression.
* Pass custom functions to be used within the expression.


## Supported Host Languages

* [Python](python)


## Data Types

* int - 64-bit signed integer
* double - 64-bit floating point number
* string - UTF-8 encoded string
* function - fixed or variable number of arguments
* array - variable types
* object - key-value


## Variables



## Operations

In the order of highest to lowest precedence:

| Operator                                                                                              | Associativity<sup>1</sup> | Description                                                     |
| ----------------------------------------------------------------------------------------------------- | ------------------------- | --------------------------------------------------------------- |
| `++` `--`                                                                                             |                           | Postfix increment, decrement                                    |
| `++` `--`                                                                                             |                           | Prefix increment, decrement                                     |
| `!`<br>`~`<br>`+` `-`                                                                                 |                           | Logical not<br>Bitwise not<br>Positive, negative                |
| `**`                                                                                                  | right-to-left             | Power                                                           |
| `*` `/` `%`                                                                                           |                           | Multiply, divide, modulus                                       |
| `+` `-`                                                                                               |                           | Add, subtract                                                   |
| `<<`<br>`>>`<br>`>>>`                                                                                 |                           | Left shift<br>Right shift sign-extended<br>Right shift unsigned |
| `<` `>` `<=` `>=`                                                                                     |                           | Inequality                                                      |
| `==` `!=`                                                                                             |                           | Equal to, not equal to                                          |
| `&`                                                                                                   |                           | Bitwise and                                                     |
| `^`                                                                                                   |                           | Bitwise xor                                                     |
| `\|`                                                                                                  |                           | Bitwise or                                                      |
| `&&`                                                                                                  |                           | Logical and                                                     |
| `\|\|`                                                                                                |                           | Logical or                                                      |
| `=`<br>`**=`<br>`*=` `/=` `%=`<br>`+=` `-=`<br>`<<=` `>>=` `>>>=`<br>`&=` `^=` `\|=`<br>`&&=` `\|\|=` | right-to-left             | Assignment                                                      |
| `? ... :`                                                                                             | right-to-left             | Ternary conditional                                             |

<sup>1</sup> Associativity is left-to-right unless noted otherwise.



## Built-In Functions

* `CEIL(int or double) -> int`
* `EVAL(string) -> any`
* `FLOOR(int or double) -> int`
* `FOR(expr1, expr2, expr3, expr4) -> any`
* `FUNCTION(string, expr) -> function`
* `IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`
* `LEN(string or array or object) -> int`
* `PRINT(any, [any, [any, ...]]) -> int`
* `ROUND(int or double) -> int`
* `SQRT(int or double) -> double`
* `WHILE(expr1, expr2) -> any`


## License

[Apache 2.0](LICENSE)

