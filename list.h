
#define NIL -1

typedef struct {
	void *id;
	void *dato;
} tNodo;

typedef int tPosL;

typedef struct {
	int MAX;
	tNodo *Array;
	tPosL fin;
}tList;

void createEmptyList(tList* list, int tam);
int isEmptyList (tList list);
tPosL first (tList list);
tPosL last (tList list);
tPosL next (tPosL pos, tList list);
tPosL previous (tPosL pos, tList list);
int insertItem (tNodo item, tPosL pos, tList* l);
tPosL findItem(char *id, tList list);
void deleteAtPosition (tPosL pos, tList* list);
void updateItem(tList* list, tPosL pos, tNodo nodo);
tNodo getItem (tPosL pos, tList list);
int copyList(tList *list1, tList list2);