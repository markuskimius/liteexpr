.PHONY: all clean install

BINARIES=le-runner 00-example 01-operations 02-builtins
CC=$(CXX)
CXXFLAGS=-I/usr/local/include/antlr4-runtime -I.. -std=c++17
LDFLAGS=-L..
LDLIBS=-lantlr4-runtime -lliteexpr

all: $(BINARIES)

le-runner: le-runner.o ../libliteexpr.a

le-runner.o: le-runner.cpp ../liteexpr.h

00-example: 00-example.o ../libliteexpr.a

00-example.o: 00-example.cpp ../liteexpr.h

01-operations: 01-operations.o ../libliteexpr.a

01-operations.o: 01-operations.cpp ../liteexpr.h

02-builtins: 02-builtins.o ../libliteexpr.a

02-builtins.o: 02-builtins.cpp ../liteexpr.h

clean:
	$(RM) $(BINARIES) *.o

# vim:noet
