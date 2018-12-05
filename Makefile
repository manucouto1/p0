main: main.c list.o
	gcc -Wall -o main main.c list.o

list.o: list.c list.h
	gcc -Wall -c list.c

.PHONY: limpiar
limpiar:
	rm list.o main
