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


def main():
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


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    main()


# vim:filetype=python:
