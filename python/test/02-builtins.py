#!/bin/bash

##############################################################################
# BOOTSTRAP
#
# Use python3 or python, whichever is available.
# (See https://unix.stackexchange.com/questions/20880)
#
if "true" : '''\'
then
    exec $(command -v python3 || command -v python) "$0" "$@"
    exit 127
fi
'''


##############################################################################
# PYTHON CODE BEGINS HERE

import sys
import liteexpr

BUILTINS = [
    "CEIL",
    "EVAL",
    "FLOOR",
    "FOR",
    "FUNCTION",
    "IF",
    "LEN",
    "PRINT",
    "ROUND",
    "SQRT",
    "WHILE",
]


def main():
    nan = float("NaN")
    inf = float("Inf")
    ninf = float("-Inf")

    for i in [ 0, 7, 3.5, -7, -3.5, nan, inf, ninf, '""', '"x"' ]:
        for builtin in BUILTINS:
            symbols = liteexpr.SymbolTable({ "i":i })
            expr = f"{builtin}(i)"

            try:
                result = liteexpr.eval(expr, symbols)
            except liteexpr.Error as e:
                result = str(e).replace("\n", "")

            expr = f"{builtin}({i})"
            print(f"{expr:<15} => {result}")


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    main()


# vim:filetype=python:
