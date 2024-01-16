all: lib/LiteExprVisitor.py

lib/LiteExprVisitor.py: ../LiteExpr.g4
	antlr4 -Dlanguage=Python3 -visitor -no-listener -Xexact-output-dir $^ -o lib

dist: lib/LiteExprVisitor.py
	./setup.py sdist

upload: dist
	python3 -m twine upload dist/*.tar.gz

install: lib/LiteExprVisitor.py
	pip3 install .

uninstall:
	pip3 uninstall liteexpr

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

gui7:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/007-*.le

test1: lib/LiteExprVisitor.py
	./test.py ../test/001-*.le

test2: lib/LiteExprVisitor.py
	./test.py ../test/002-*.le

test3: lib/LiteExprVisitor.py
	./test.py ../test/003-*.le

test4: lib/LiteExprVisitor.py
	./test.py ../test/004-*.le

test5: lib/LiteExprVisitor.py
	./test.py ../test/005-*.le

test6: lib/LiteExprVisitor.py
	./test.py ../test/006-*.le

test7: lib/LiteExprVisitor.py
	./test.py ../test/007-*.le

clean:
	rm -rf lib/LiteExpr*.py lib/*.interp lib/*.tokens lib/__pycache__ build dist

# vim:noet
