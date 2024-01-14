# liteexpr

A light, expression language.


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

| Operator           | Associativity[^1] | Description                   |
| ------------------ | ----------------- | ----------------------------- |
| `++` `--`          |                   | Postfix increment, decrement  |
| ------------------ | ----------------- | ----------------------------- |
| `++` `--`          |                   | Prefix increment, decrement   |
| ------------------ | ----------------- | ----------------------------- |
| `!`                |                   | Logical not                   |
| `~`                |                   | Bitwise not                   |
| `+` `-`            |                   | Positive, negative            |
| ------------------ | ----------------- | ----------------------------- |
| `**`               | right-to-left     | Power                         |
| ------------------ | ----------------- | ----------------------------- |
| `*` `/` `%`        |                   | Multiply, divide, modulus     |
| ------------------ | ----------------- | ----------------------------- |
| `+` `-`            |                   | Add, subtract                 |
| ------------------ | ----------------- | ----------------------------- |
| `<<`               |                   | Left shift                    |
| `>>`               |                   | Right shift sign-extended     |
| `>>>`              |                   | Right shift unsigned          |
| ------------------ | ----------------- | ----------------------------- |
| `<` `>` `<=` `>=`  |                   | Inequality                    |
| ------------------ | ----------------- | ----------------------------- |
| `==` `!=`          |                   | Equal to, not equal to        |
| ------------------ | ----------------- | ----------------------------- |
| `&`                |                   | Bitwise and                   |
| ------------------ | ----------------- | ----------------------------- |
| `^`                |                   | Bitwise xor                   |
| ------------------ | ----------------- | ----------------------------- |
| `|`                |                   | Bitwise or                    |
| ------------------ | ----------------- | ----------------------------- |
| `&&`               |                   | Boolean and                   |
| ------------------ | ----------------- | ----------------------------- |
| `||`               |                   | Boolean or                    |
| ------------------ | ----------------- | ----------------------------- |
| `=`                | right-to-left     | Assignment                    |
| `**=`              |                   |                               |
| `*=` `/=` `%=`     |                   |                               |
| `+=` `-=`          |                   |                               |
| `<<=` `>>=` `>>>=` |                   |                               |
| `&=` `^=` `|=`     |                   |                               |
| `&&=` `||=`        |                   |                               |
| ------------------ | ----------------- | ----------------------------- |
| `? ... :`          |                   | Ternary conditional           |

[^1] Associativity is left-to-right unless noted otherwise.


## Built-In Functions

* `CEIL(int or double) -> int`
* `EVAL(string) -> any`
* `FLOOR(int or double) -> int`
* `FOR(expr1, expr2, expr3, expr4) -> any`
* `IF(expr1, then1, [expr2, then2, [expr3, then3, ...]], [else]) -> any`
* `LEN(string or array or object) -> int`
* `PRINT(expr1, [expr2, [expr3, ...]]) -> int`
* `ROUND(int or double) -> int`
* `SQRT(int or double) -> double`
* `WHILE(expr1, expr2) -> any`


## License

[Apache 2.0](LICENSE)
