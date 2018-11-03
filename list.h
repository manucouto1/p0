#include <stddef.h>
#include <bits/types/time_t.h>
#include <bits/types/struct_tm.h>

#define MAX 4096
#define NIL -1

typedef void *tAddr;
typedef enum { mallocc = 0, mmapp = 1, shared = 2} tType;

typedef struct {
	struct tm fecha;
	tType tipo;
	size_t size;
	void* addr;
	void* extra;
} tNodo;

typedef struct {
	int fd;
	char* fich;
} mmap_info;

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
tPosL findItem(char* addr, tList list);
void deleteAtPosition (tPosL pos, tList* list);
void updateItem(tList* list, tPosL pos, tNodo nodo);
tNodo getItem (tPosL pos, tList list);