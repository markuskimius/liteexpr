# liteexpr

A light, expression language.


## Language Overview

For the language overview, see the [main page](https://github.com/markuskimius/liteexpr).
The rest of the document describes using liteexpr with C++.


## Installation

First, install Antlr 4.13.1 to `/usr/local/include/antlr4-runtime` and
`/usr/local/lib/libantlr4-runtime.a` and delete
`/usr/local/lib/libantlr4-runtime.{so,dylib}`

Then `make`.  Copy `*.h` and `libliteexpr.a` where you'd like.


## Example

```cpp
#include <string>
#include <iostream>
#include "liteexpr.h"

int main(int argc, const char* argv[]) {
    liteexpr::SYMBOLS symbols = liteexpr::make_symbols({
        { "grades", liteexpr::make_value({
            { "alice", liteexpr::make_value("A") },
            { "bob"  , liteexpr::make_value("B") },
        })},
    });

    try {
        liteexpr::eval(R"(
            PRINT("I have " + LEN(grades) + " students");
            PRINT("Alice's grade is " + grades.alice);
        )", symbols);
    }
    catch(liteexpr::Error e) {
        std::cerr << std::string(e) << std::endl;
        return 1;
    }

    return 0;
}
```

Output:

```
I have 2 students
Alice's grade is A
```


## License

[Apache 2.0](https://github.com/markuskimius/liteexpr/blob/main/LICENSE)

