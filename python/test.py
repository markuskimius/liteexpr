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
    for arg in sys.argv[1:]:
        with open(arg) as fd:
            doMyThing(fd)

    if len(sys.argv[1:]) == 0:
        doMyThing(sys.stdin)


def doMyThing(fd):
    output = liteexpr.evalfd(fd)

    print(output)


##############################################################################
# ENTRY POINT

if __name__ == "__main__":
    main()


# vim:filetype=python:
