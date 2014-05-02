run: plusy
	./plusy

signal: main.c
	clang --std=c11 -Wall -pedantic -g main.c -o plusy