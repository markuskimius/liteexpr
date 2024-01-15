# liteexpr

A light, expression language.


## Language Overview

For the language overview, see the [main page](..).
The rest of the document describes using liteexpr with Python.


## Installation

```sh
pip3 install liteexpr
```


## Usage

```python
import liteexpr

grades = {
    "alice" : "A",
    "bob"   : "B",
}

liteexpr.eval("""
    PRINT("I have " + LEN(grades) + " students");
    PRINT("Alice's grade is " + grades.alice)
""", grades)
```


## License

[Apache 2.0](../LICENSE)

