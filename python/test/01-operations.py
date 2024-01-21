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

PREOPS = [ "!", "~", "+", "-", "++", "--" ]
POSTOPS = [ "++", "--" ]
BINARYOPS = [
    '**', '*', '/', '%', '+', '-', '<<', '>>', '>>>', '<', '<=', '>', '>=',
    '==', '!=', '&', '^', '|', '&&', '||'
]


def main():
    nan = float("NaN")
    inf = float("Inf")
    ninf = float("-Inf")

    for i in [ 0, 7, 3.5, -7, -3.5, nan, inf, ninf, '""', '"x"' ]:
        for op in PREOPS:
            symbols = liteexpr.SymbolTable({ "i":i })
            expr = f"{op}i"

            try:
                result = liteexpr.eval(expr, symbols)
            except liteexpr.Error as e:
                result = str(e)

            print(f"{'':>4} {op:>3} {i:>4} => {result}")

    for i in [ 0, 7, 3.5, -7, -3.5, nan, inf, ninf, '""', '"x"' ]:
        for op in POSTOPS:
            symbols = liteexpr.SymbolTable({ "i":i })
            expr = f"i{op}"

            try:
                result = liteexpr.eval(expr, symbols)
            except liteexpr.Error as e:
                result = str(e)

            print(f"{i:>4} {op:>3} {'':>4} => {result}")

    for i in [ 0, 0.0, 7, 3.5, -7, -3.5, nan, inf, ninf, '""', '"x"' ]:
        for op in BINARYOPS:
            for j in [ 0, 3, 3.5, -4, -4.5, nan, inf, ninf, '"x"', '"y"' ]:
                symbols = liteexpr.SymbolTable({ "i":i, "j":j })
                expr = f"i {op} j"

                try:
                    result = liteexpr.eval(expr, symbols)
                except liteexpr.Error as e:
                    result = str(e)

                print(f"{i:>4} {op:>3} {j:>4} => {result}")


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    main()


# vim:filetype=python:
