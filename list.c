#include "list.h"
#include <stdio.h>
#include <memory.h>

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

tPosL findItem (tAddr addr, tList l) {
	tPosL p;

	if (!isEmptyList(l)) {
		p = first(l);
		while ((p != NIL) && (memcmp(addr, l.Array[p].addr, 100*sizeof(tAddr)))) {
			p = next(p, l);
		}
		return p;
	}
	else return NIL;
}

void deleteAtPosition (tPosL p, tList* l) {
	tPosL i;

	for (i = p; i <= l->fin - 1; i++) {
		l->Array[i] = l->Array[i + 1];
	}
	l->fin = l->fin - 1;
}

void updateItem (tList* l, tPosL p, tNodo nodo) {
	l->Array[p] = nodo;
}

tNodo getItem (tPosL p, tList l) {
	return l.Array[p];
}
