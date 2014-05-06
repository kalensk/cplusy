all: clean plusy run

clean:
	rm -f ./plusy
	rm -rf ./plusy.dSYM

run: plusy
	./plusy

plusy: main.c
	clang --std=c11 -Wall -pedantic -g main.c -o plusy