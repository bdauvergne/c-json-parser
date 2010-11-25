test: test.o json_parser.o

all: test

clean:
	-rm -f test *.o
