.PHONY: all clean dist upload install uninstall gui test

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

gui:
	$(if $(TEST),,$(error Usage: make $@ TEST=<test#>))
	antlr4-parse ../LiteExpr.g4 file -gui ../test/$(shell printf "%02d" $(TEST))-*.le

test: lib/LiteExprVisitor.py
	$(if $(TEST),,$(error Usage: make $@ TEST=<test#>))
	./test/le-runner.py ../test/$(shell printf "%02d" $(TEST))-*.le

clean:
	rm -rf lib/LiteExpr*.py lib/*.interp lib/*.tokens lib/__pycache__ build dist

# vim:noet
