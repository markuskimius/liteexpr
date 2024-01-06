.PHONY: all clean python

all: python

python:
	make -C $@

clean:
	make -C python $@

# vim:noet
