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

int insertItem (tNodo nodo, tPosL pos, tList* l) {
	tPosL i;

	if (l->fin == MAX) {
		return 0;
	}
	else {
		l->fin = l->fin + 1;
		if (pos == NIL) {
			l->Array[l->fin] = nodo;
		}
		else {
			for (i = l->fin; i >= pos + 1; i--) {
				l->Array[i] = l->Array[i - 1];
			}
			l->Array[pos] = nodo;
		}
		return 1;
	}
}

tPosL findItem (tNodo nodo, tList l) {
	tPosL p;

	p = first(l);
	while ((p < l.fin) && (l.Array[p].id != nodo.id)) {
		p = next(p, l);
	}

	if (p < l.fin) {
		return p;
	}
	else {
		if (p == l.fin) {
			if (nodo.id == l.Array[p].id) {
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

void updateItem (tList* l, tPosL p,tNodo nodo) {
	l->Array[p] = nodo;
}

tNodo getItem (tPosL p, tList l) {
	return l.Array[p];
}
