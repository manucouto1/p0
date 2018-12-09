#include "list.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>


void createEmptyList(tList* l, int tam, tIdType tId) {

	l->MAX = tam;
	l->Array = malloc(tam* sizeof(tNodo));
	l->fin = NIL;
	l->tId = tId;
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
			switch (l.tId){
				case 0:
					sprintf(aux1,"%"PRIxPTR"", (intptr_t ) l.Array[p].id);
					b = (!strcmp(aux1,id));
					break;
				case 1:
					//sprintf(aux1,"%s", (char *) l.Array[p].id );
					b = (!strcmp(l.Array[p].id,id));
					break;
				case 2:
				default:
					sprintf(aux1,"%p", l.Array[p].id);
					b = (!strcmp(aux1,id));
					break;
			}

			if(!b) p = next(p, l);
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

int copyList(tList *list1, tList list2){

	list1->fin = list2.fin;
	list1->MAX = list2.MAX;
	list1->tId = list2.tId;
	list1->Array = list2.Array;
	return 0;
}
