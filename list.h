
#define MAX 4096
#define NIL -1

typedef struct {
	void *id;
	void *dato;
	void *tipo
} tNodo;

typedef  struct {
	int key;
} shared_info;

typedef int tPosL;

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
int insertItem (tNodo item, tPosL pos, tList* l);
tPosL findItem(void *id, tList list);
void deleteAtPosition (tPosL pos, tList* list);
void updateItem(tList* list, tPosL pos, tNodo nodo);
tNodo getItem (tPosL pos, tList list);