#include <stddef.h>

#define MAX 4096
#define NIL 0

typedef size_t tSize;
typedef char *tTime, *tType, *tFich;
typedef void *tAddr; //, *tStruct;
typedef int tFd, tKey;
/*typedef struct {
	char *fich;
	int fd;
} mmap;

struct shared {
	int key;
};*/

typedef struct {
	tAddr addr;
	tSize size;
	tTime time;
	tType type;
	tFich fich;
	tFd fd;
	tKey key;
}tNodo;

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
tPosL findItem(tType dato, tSize tam, tList list);
void deleteAtPosition (tPosL pos, tList* list);
void updateItem(tList* list, tPosL pos, tNodo nodo);
tNodo getItem (tPosL pos, tList list);