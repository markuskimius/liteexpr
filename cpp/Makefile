.PHONY: all clean test

TARGET=libliteexpr.a
CXXFLAGS=-I/usr/local/include/antlr4-runtime -std=c++17
LDLIBS=-lantlr4-runtime

all: $(TARGET)

LiteExpr.interp LiteExpr.tokens LiteExprBaseVisitor.cpp LiteExprBaseVisitor.h LiteExprLexer.cpp LiteExprLexer.h LiteExprLexer.interp LiteExprLexer.tokens LiteExprParser.cpp LiteExprParser.h LiteExprVisitor.cpp LiteExprVisitor.h: ../LiteExpr.g4
	antlr4 -Dlanguage=Cpp -visitor -no-listener -Xexact-output-dir $^ -o .

libliteexpr.a: liteexpr.o LiteExprBaseVisitor.o LiteExprLexer.o LiteExprParser.o LiteExprVisitor.o
	ar rc $@ $^

liteexpr.o: liteexpr.cpp liteexpr.h LiteExprBaseVisitor.h

LiteExprBaseVisitor.o: LiteExprBaseVisitor.cpp LiteExprBaseVisitor.h

LiteExprLexer.o: LiteExprLexer.cpp LiteExprLexer.h

LiteExprParser.o: LiteExprParser.cpp LiteExprParser.h

LiteExprVisitor.o: LiteExprVisitor.cpp LiteExprVisitor.h

test: test/le-runner
	$(if $(TEST),,$(error Usage: make $@ TEST=<test#>))
	./test/le-runner ../test/$(shell printf "%02d" $(TEST))-*.le

test/le-runner:
	make -C test

clean:
	make -C test clean
	$(RM) LiteExpr*.cpp LiteExpr*.h *.interp *.tokens *.o $(TARGET)

# vim:noet
