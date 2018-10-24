//#include <list.h>
#include <stdio.h>

#define NIL 0
/*#define max 4096

typedef void* tDato;

typedef struct {
	tDato dato;
} tNodo;

typedef int tPosL;

typedef struct {
	tNodo array[max];
	tPosL fin;
} tList;*/

void createEmptyList(tList* l) {
	l->fin = NIL;
}

int isEmptyList(tList l) {
	return l.fin == NIL;
}

tPosL first(tList l) {
	return 0;
}

tPosL last(tList l) {
	return l.fin;
}

tPosL previous(tPosL p, tList l) {
	if (p > first(l)) {
		return p -1;
	}
	else return NIL;
}

tPosL next(tPosL p, tList l) {
	if (p < last(l)) {
		return p + 1;
	}
	else return NIL;
}

int insertItem (tNodo item, tPosL pos) {

}

int main() {
	tList l;
	createEmptyList(&l);

	printf("%d", l.fin);
}