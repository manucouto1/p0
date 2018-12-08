main: main.c list.o
	gcc -g -Wall -o main main.c list.o

list.o: list.c list.h
	gcc -g -Wall -c list.c

.PHONY: limpiar
limpiar:
	rm list.o main
