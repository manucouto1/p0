#include "list.h"
#include <stdio.h>

#define NIL 0

void createEmptyList(tList* l) {
	l->fin = NIL;
}

int isEmptyList(tList l) {
	return l.fin == NIL;
}

tPosL first(tList l) {
	return 1;
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

int insertItem (tNodo item, tPosL pos, tList* l) {
	tPosL i;

	if (l->fin == MAX) {
		return 0;
	}
	else {
		l->fin = l->fin + 1;
		if (pos == NIL) {
			l->Array[l->fin] = item;
		}
		else {
			for (i = l->fin; i >= pos + 1; i--) {
				l->Array[i] = l->Array[i - 1];
			}
			l->Array[pos] = item;
		}
		return 1;
	}
}

tPosL findItem (tType dato, tList l) {
	tPosL p;

	p = first(l);
	while ((p < l.fin) && (l.Array[p].type != dato)) {
		p = next(p, l);
	}

	if (p < l.fin) {
		return p;
	}
	else {
		if (p == l.fin) {
			if (dato == l.Array[p].type) {
				return p;
			}
		}
	}
	return NIL;
}

void deleteAtPosition (tPosL p, tList* l) {
	tPosL i;

	for (i = p; i <= l->fin - 1; i++) {
		l->Array[i] = l->Array[i - 1];
	}
	l->fin = l->fin - 1;
}

void updateItem (tList* l, tPosL p, tNodo nodo) {
	l->Array[p] = nodo;
}

tNodo getItem (tPosL p, tList l) {
	return l.Array[p];
}

/*int main() {
	tList l;
	createEmptyList(&l);
	tNodo c ;

	for (int i = 1; i <= 10; i++ ) {
		c.dato = i;
		if (insertItem(c, NIL, &l)) {
			printf("Jorge Ponce\n");
		}
		printf("%d\n", getItem(i, l).dato);
	}

	printf("%d\n", first(l));
	printf("%d\n", last(l));
	printf("%d\n", previous(last(l), l));
	printf("%d\n", next(last(l), l));
}*/