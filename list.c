#include "list.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>


void createEmptyList(tList* l, int tam) {
	l->MAX = tam;
	l->Array = malloc(tam* sizeof(tNodo));
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

	if (l->fin == l->MAX) {
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

tPosL findItem (char *id, tList l) {
	tPosL p;
	int b = 0;
	char aux1[50];

	if (!isEmptyList(l)) {
		p = first(l);
		while ((p != NIL) && !b) {
			sprintf(aux1,"%p", l.Array[p].id);
			if (!strcmp(aux1,id)) {
				b = 1;
			} else p = next(p, l);
		}
		return p;
	}
	else {
		return NIL;
	}
}

void deleteAtPosition (tPosL p, tList *l) {
	tPosL i;

	for (i = p; i <= (l->fin); i++) {
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
