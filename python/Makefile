all: LiteExpr.tokens

LiteExpr.tokens: ../LiteExpr.g4
	antlr4 -Dlanguage=Python3 -Xexact-output-dir $^ -o .

gui1:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/001-*.le

gui2:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/002-*.le

gui3:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/003-*.le

gui4:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/004-*.le

gui5:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/005-*.le

gui6:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/006-*.le

test1: LiteExpr.tokens
	./test.py ../test/001-*.le

test2: LiteExpr.tokens
	./test.py ../test/002-*.le

test3: LiteExpr.tokens
	./test.py ../test/003-*.le

test4: LiteExpr.tokens
	./test.py ../test/004-*.le

test5: LiteExpr.tokens
	./test.py ../test/005-*.le

test6: LiteExpr.tokens
	./test.py ../test/006-*.le

clean:
	rm -rf LiteExpr*.py *.interp *.tokens __pycache__

# vim:noet
