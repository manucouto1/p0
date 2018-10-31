#define MAX 6400

typedef int tPosL;

typedef struct {
	void *id;
	void *dato;
}tNodo;

typedef struct {
	tNodo Array[MAX];
	tPosL fin;
}tList;

void createEmptyList(tList* list);
int isEmptyList (tList list);
tPosL first (tList list);
tPosL last (tList list);
tPosL next (tPosL pos, tList list);
tPosL previous (tPosL pos, tList list);
int insertItem (tNodo, tPosL pos, tList* l);
tPosL findItem(tNodo, tList list);
void deleteAtPosition (tPosL pos, tList* list);
void updateItem(tList* list, tPosL pos, tNodo);
tNodo getItem (tPosL pos, tList list);