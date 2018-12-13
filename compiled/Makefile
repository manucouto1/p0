main: ../main.c list.o senales.o
	gcc -g -Wall -o main ../main.c list.o senales.o

list.o: ../list.c ../list.h
	gcc -g -Wall -c ../list.c

senales.o: ../senales.c ../senales.h
	gcc -g -Wall -c ../senales.c

.PHONY: limpiar
limpiar:
	rm list.o senales.o main