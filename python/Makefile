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

gui0:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/000-*.le

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

gui8:
	antlr4-parse ../LiteExpr.g4 file -gui ../test/008-*.le

test0: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/00-*.le

test1: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/01-*.le

test2: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/02-*.le

test3: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/03-*.le

test4: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/04-*.le

test5: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/05-*.le

test6: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/06-*.le

test7: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/07-*.le

test8: lib/LiteExprVisitor.py
	./test/le-runner.py ../test/08-*.le

clean:
	rm -rf lib/LiteExpr*.py lib/*.interp lib/*.tokens lib/__pycache__ build dist

# vim:noet
