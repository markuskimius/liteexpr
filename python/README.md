# liteexpr for Python

This document describes using liteexpr with Python.
For the language overview, see the [main page](https://github.com/markuskimius/liteexpr).


## Installation

```sh
pip3 install liteexpr
```

Python 3.8 and later are supported.


## Example

```python
import liteexpr

symbols = liteexpr.SymbolTable({
    "grades" : {
        "alice" : "A",
        "bob"   : "B",
    }
})

liteexpr.eval("""
    PRINT("I have " + LEN(grades) + " students");
    PRINT("Alice's grade is " + grades.alice);
""", symbols)
```

Output:

```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](https://github.com/markuskimius/liteexpr/blob/main/LICENSE)

