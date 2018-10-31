#include "list.h"
#include <stdio.h>

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

tPosL findItem (tType dato, tSize tam, tList l) {
	tPosL p;

	p = first(l);

	while ((p != NIL) && (l.Array[p].type != dato) && (l.Array[p].size != tam)) {
		p = next(p, l);
	}

	return p;
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
