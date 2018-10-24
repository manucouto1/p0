#define MAX 6400

typedef void *tDato;

typedef struct {
	tDato dato;
}tNodo;

typedef int tPosL;

typedef struct {
	tNodo Array[MAX];
	tPosL fin;
}tList;

int createEmptyList(tList list);
int isEmptyList (tList list);
tPosL first (tList list);
tPosL last (tList list);
tPosL next (tPosL pos, tList list);
tPosL previous (tPosL pos, tList list);
int insertItem (tList list);
tPosL findItem(tNodo element, tList list);
int deleteAtPosition (tPosL pos, tList list);
int updateItem(tList list, tPosL pos, tNodo nodo);
tNodo getItem (tPosL pos, tList list);