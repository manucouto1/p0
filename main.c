/*
 * Autores:
 * Manuel Couto Pintos, login: manuel.couto1@udc.es, DNI: 77462284D
 * Víctor Escudero González, login: v.escudero@udc.es, DNI: 54153242H
 */
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <wait.h>

#include "list.h"

#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2
#define ERROR_DELETING_FILE -3
#define ERROR_DELETING_DIR -4
#define ERROR_LISTING -5
#define ERROR_INSERT -6
#define ERROR_IPC -7
#define ERROR_READING -8
#define ERROR_FORK -9


#define clear() printf("\033[H\033[J")

#define MAXSEARCHLIST 128
#define MAXNOMBRE 1024
#define MAXALLOC 4096

typedef enum {mallocc = 0, mmapp = 1, shared = 2} tType;
typedef enum {running = 0, stopped = 1, termNormally = 2, termBySignal = 3} tTypeProc;

typedef struct {
	tList lista;
	tList searchList;
	tList listaBackground;
	int nargs;
	char *flags[1024];
}container;

typedef struct {
	size_t size;
	char date[256];
	tType tipo;
	void *extra;
}tDato;

typedef struct {
	int fd;
	char fichero[256];
}tMap;

typedef struct {
	key_t key;
}tShared;

typedef struct {
	int prioridad;
	char *comando;
	char *fechaInicio;
	tTypeProc status;
	void* valorProc;
} tProcess;

struct element_description{
	uintmax_t nInodo;
	char permisos[12];
	long links;
	char *propietario;
	char *grupo;
	char name[256];
	long size;
	char fecha[200];
};

typedef struct {
	int key;
	size_t tam;
	char file[256];
} auxDealloc;

int global1;
int global2;
int global3;

int trocearCadena ( char * cadena, char * trozos[]){
	int  i = 1;

	if((trozos[0] = strtok(cadena, " \n\t")) == NULL)
		return 0;
	while ((trozos[i]=strtok(NULL, " \n\t")) != NULL)
		i++;
	return i;

}

void imprimirPrompt(){
	printf("\n $ ");
}

void leerEntrada( char * cadena){
	fgets(cadena, 1024, stdin);
}

char TipoFichero (mode_t m)
{
	switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
		case S_IFSOCK: return 's'; /*socket */
		case S_IFLNK: return 'l'; /*symbolic link*/
		case S_IFREG: return '-'; /* fichero normal*/
		case S_IFBLK: return 'b'; /*block device*/
		case S_IFDIR: return 'd'; /*directorio */
		case S_IFCHR: return 'c'; /*char device*/
		case S_IFIFO: return 'p'; /*pipe*/
		default: return '?'; /*desconocido, no deberia aparecer*/
	}
}

char * ConvierteModo2 (mode_t m)
{
	static char permisos[12];
	strcpy (permisos,"---------- ");
	permisos[0]=TipoFichero(m);
	if (m&S_IRUSR) permisos[1]='r'; /*propietario*/
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r'; /*grupo*/
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r'; /*resto*/
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s'; /*setuid, setgid y stickybit*/
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';
	return (permisos);
}


int cmd_autores(container *c){
	int i = 1;

	char *autores[2] = {"Manuel Couto Pintos","Victor Escudero Gonzalez"};
	char *correos[2] = {"manuel.couto1@udc.es", "victor.escudero@udc.es"};

	switch(c->nargs){
		case 1 :
			printf("%s\t%s\n", autores[0], autores[1]);
			printf("%s\t%s\n", correos[0], correos[1]);
			break;
		case 2 :
			if(!strcmp(c->flags[i],"-n")){
				printf("%s\t%s\n", autores[0], autores[1]);
			} else if(!strcmp(c->flags[i],"-l")) {
				printf("%s\t%s\n", correos[0], correos[1]);
			} else {
				return COMANDO_INVALIDO;
			}
			break;
		default :
			return COMANDO_INVALIDO;
	}

	return 0;
}

int cmd_pid(container *c) {
	switch (c->nargs) {
		case 1:
			printf("Process ID: %d", getpid());
			return 0;
		case 2:
			if (!strcmp(c->flags[1],"-p")) {
				printf("PPID: %d", getppid());
				return 0;
			}
			else {
				return COMANDO_INVALIDO;
			}
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_hora (container *c){
	struct tm *t;
	char fecha[200];
	time_t s;
	time(&s);
	t = localtime(&s);
	strftime(fecha, sizeof(fecha), "%H:%M", t);
	switch (c->nargs) {
		case 1 :
			printf("%s", fecha);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_fecha (container *c) {
	struct tm *t;
	char fecha[200];
	time_t s;
	time(&s);
	t = localtime(&s);
	strftime(fecha, sizeof(fecha), "%d/%m/%Y", t);
	switch (c->nargs) {
		case 1:
			printf("%s",fecha);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_chdir(container *c){
	char dir[1024];

	switch (c->nargs) {
		case 1:
			getcwd(dir,1024);
			printf("%s",dir);
			return 0;
		case 2:

			if(chdir(c->flags[1])!=0){
				sprintf(dir," chdir: %s", strerror(errno));
				printf("%s", dir);
			};
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_create(container *c) {
	int fd;

	switch (c->nargs) {
		case 2:
			fd = open(c->flags[1], O_WRONLY | O_APPEND | O_CREAT, 0644);
			close(fd);
			return 0;
		case 3:
			if(!strcmp(c->flags[1],"-d")){
				if(!mkdir(c->flags[2],0777)){
					return 0;
				} else {
					printf("cannot create: %s\n", strerror(errno));
					return ERROR_CREATING_FILE;
				}
			} else {
				return COMANDO_INVALIDO;
			}
		default:
			return COMANDO_INVALIDO;
	}
}

/* - DELETE util functions */

int borrar_archivo(char* elemento){
	if (unlink(elemento)) {
		printf("cannot delete file %s: %s\n", elemento, strerror(errno));
		return ERROR_DELETING_FILE;
	} else {
		printf("\n Borrando archivo %s",elemento);
		return 0;
	}
}

int borrar_directorio(char * elemento){
	if (rmdir(elemento)) {
		printf("cannot delete directory %s: %s\n", elemento, strerror(errno));
		return ERROR_DELETING_DIR;
	} else {
		printf("\n Borrando directorio %s",elemento);
		return 0;
	}
}

int borrar_rec(char *elemento, struct stat path_stat){

	DIR *dir;
	struct dirent *ent;

	if (S_ISDIR(path_stat.st_mode)) {
		if ((dir = opendir(elemento)) != NULL) {
			chdir(elemento);
			while ((ent = readdir(dir)) !=NULL) {
				if (strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0) {
					stat(ent->d_name, &path_stat);
					borrar_rec(ent->d_name, path_stat);
				}
			}
			chdir("..");
			closedir(dir);
			borrar_directorio(elemento);
		}
	} else {
		borrar_archivo(elemento);
	}
	return 0;
}
/* - end DELETE util functions */

/*
 * DONE - Eliminar un fichero o un directorio
 * DONE - Si no tiene flag el directorio tiene que estar vacio para ser borrado
 * DONE - Si tiene un flag -r borrar el contenido y el directorio
 * DONE - Si no hay argumentos no se hace nada
 * DONE - Si no se puede eliminar Hay que informar al usuario con un mensaje
 */
int cmd_delete(container *c) {

	struct stat path_stat;

	switch ( c->nargs ) {
		case 1:
			return 0;
		case 2:
			if (!stat(c->flags[1], &path_stat)) {
				if (S_ISDIR(path_stat.st_mode)) {
					return borrar_directorio(c->flags[1]);
				} else {
					return borrar_archivo(c->flags[1]);
				}
			}
			else {
				printf("cannot delete %s: %s\n",c->flags[1], strerror(errno));
			}
		case 3:
			if (!strcmp(c->flags[1], "-r")) {
				if (!stat(c->flags[2], &path_stat)) {
					borrar_rec(c->flags[2], path_stat);
				}
				else {
					printf("cannot delete %s: %s\n",c->flags[2], strerror(errno));
				}
			} else {
				return COMANDO_INVALIDO;
			}
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

/* - LIST/QUERY util functions */

int load_data(char *element, struct element_description *description){

	struct stat fileStat;
	struct tm *tm;
	FILE *file;
	char buf[200];
	char fich[1024], current[1024];
	getcwd(current, 1024);

	if((lstat(element, &fileStat)==0) && ((file=fopen(element,"r"))!= NULL)) {

		strcpy(description->name, element);
		if (S_ISLNK(fileStat.st_mode)) {
			chdir(element);
			strcat(description->name," -> ");
			strcat(description->name, getcwd(fich, 1024));
			chdir(current);
		}
		strcpy(description->permisos, ConvierteModo2(fileStat.st_mode));
		description->propietario = getpwuid(fileStat.st_uid) -> pw_name;
		description->grupo = getgrgid(fileStat.st_gid) -> gr_name;
		description->links = (long) fileStat.st_nlink;
		description->nInodo = fileStat.st_ino;
		time_t t = fileStat.st_mtim.tv_sec;
		tm = localtime(&t);

		strftime(buf, sizeof(buf), "%b %d %H:%M", tm);
		strcpy(description->fecha, buf);

		fseek(file, 0L, SEEK_END);
		if (S_ISDIR(fileStat.st_mode) || S_ISLNK(fileStat.st_mode)) {
			description->size = fileStat.st_blksize;;
		}
		else {
			description->size = ftell(file);
		}
		fclose(file);
		//printf("\t %s %d %d:%d ", tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);
	} else {
		printf("cannot access %s: %s\n",element, strerror(errno));
		return 0;
	}
	return 1;
}
/* - end LIST/QUERY util functions */

void print_element(struct element_description elements_description, int argN){

	if (argN) {
		printf("%8ju ", elements_description.size);
		printf("%s\n",elements_description.name);
	}
	else {
		printf("%8ju ", elements_description.nInodo);
		printf("%5s", elements_description.permisos);
		printf("%3ld ", elements_description.links);
		printf("%5s ", elements_description.propietario);
		printf("%5s ", elements_description.grupo);
		printf("%8ju ", elements_description.size);
		printf("%12s ", elements_description.fecha);
		printf("%s\n", elements_description.name);
	}
}

/*
 * DONE - Devuelve informacion de los archivos/directorios pasados como argumentos
 * DONE - Produce una linea por arch/dir pasado como argumento
 * DONE - mismo formato que ls -li resolviendo links simbolicos si es necesario
 * DONE - equivalente a ls -li para archivos y que ls -lid para directorios
 */
int cmd_query(container* c) {

	int i;
	struct element_description description;

	for(i = 1; i < c->nargs; i++){
		if (load_data(c->flags[i], &description)) {
			print_element(description, 0);
		}
	}

	return 0;
}

int fun_list_rec(char *elemento, struct stat path_stat, int nivel, int argH, int argR, int argN){

	DIR *dir;
	struct dirent *ent;
	struct element_description description;
	char current[1024];
	getcwd(current, 1024);

	if ((nivel<1) || argR) {
		if (S_ISDIR(path_stat.st_mode)) {
			if ((dir = opendir(elemento)) != NULL) {
				chdir(elemento);
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name, ".")!=0 && strcmp(ent->d_name, "..")!=0) {
						lstat(ent->d_name, &path_stat);
						if (!(!argH && (ent->d_name[0] == '.'))) {
							fun_list_rec(ent->d_name, path_stat, nivel++, argH, argR, argN);
						}
					}
				}
				closedir(dir);
				if (!strcmp(elemento, "..")) {
					chdir(current);
				}
				else if (strcmp(elemento, ".")!=0) chdir("..");
			}
		}
	}
	if (argH || (elemento[0] != '.')) {
		if (load_data(elemento, &description)) {
			print_element(description, argN);
		}
	}

	return 0;

}

int cmd_list(container* c){
	int i=1;
	int nCount=0;
	int rCount=0;
	int hCount=0;
	int nivel;
	struct stat path_stat;

	nivel = 0;

	while(i < c->nargs && (c->flags[i][0] == '-')){

		if(!strcmp(c->flags[i],"-n")){
			nCount++;
		} else if(!strcmp(c->flags[i],"-r")){
			rCount++;
		} else if(!strcmp(c->flags[i],"-h")){
			hCount++;
		} else {
			return COMANDO_INVALIDO;
		}
		i++;
	}

	int aux = hCount + rCount + nCount +1;

	if((nCount <= 1)&&(rCount <= 1)&& (hCount <= 1)){ // si solo hay flags y no hay argumento Error comando invalido
		//
		if(c->nargs!=aux){
			// Si se pasan argumentos
			for(aux = aux; aux < c->nargs; aux++){
				if (!lstat(c->flags[aux], &path_stat)) {
					fun_list_rec(c->flags[aux], path_stat, nivel, hCount, rCount, nCount);
				} else {
					printf("cannot access %s: %s\n", c->flags[aux], strerror(errno));
					return ERROR_LISTING;;
				}
			}
		} else {
			lstat(".", &path_stat);
			// Si no se pasan argumentos a mayores de los flags
			fun_list_rec(".", path_stat, nivel, hCount, rCount, nCount);
		}
		//
		return 0;
	} else {
		//COmando invalido
		return COMANDO_INVALIDO;
	}


}

void printList(tType type, tList l) {
	tNodo nodo;
	tShared shar;
	tDato dato;
	tMap map;
	tPosL i = first(l);

	if (!isEmptyList(l)) {
		while (i != NIL) {
			if ((((tDato*)(nodo = getItem(i, l)).dato)->tipo == type) || (type == -1)) {
				dato = *((tDato*) nodo.dato);
				switch ((tType) dato.tipo) {
					case mallocc:
						printf("%p: size:%lu malloc %s", nodo.id, dato.size, dato.date);
						break;
					case mmapp:
						map = *((tMap*)dato.extra);
						printf("%p: size:%lu mmap %s (fd: %d) %s", nodo.id, dato.size,
							   map.fichero, map.fd, dato.date);
						break;
					case shared:
						shar = *((tShared*)dato.extra);
						printf("%p: size:%lu shared memory (key: %d) %s", nodo.id, dato.size,
							   shar.key, dato.date);
						break;
					default:
						break;
				}
			}
			i = next(i, l);
		}
	}
}

void *MmapFichero (char *fichero, int protection, tNodo *nodo){
	int df, map=MAP_PRIVATE, modo=O_RDONLY;
	struct stat s;
	void *p;

	if(protection&PROT_WRITE) modo=O_RDWR;
	if(stat(fichero,&s)==-1 || (df=open(fichero,modo))==-1)
		return NULL;
	if((p=mmap (NULL, (size_t) s.st_size, protection, map, df, 0)) == MAP_FAILED)
		return NULL;
	// General data
	nodo->id = p;
	((tDato*)nodo->dato)->size = (unsigned long) s.st_size;
	// Especific map data
	strcpy(((tMap *)((tDato *)nodo->dato)->extra)->fichero,fichero);
	((tMap *)((tDato *)nodo->dato)->extra)->fd = df;

	return p;
}

int cmd_mmap (char* arg[], tNodo* nodo) {
	char* perm;
	int protection = 0;

	if ((perm = arg[3]) != NULL && strlen(perm) < 4) {
		if (strchr(perm, 'r') != NULL) protection|=PROT_READ;
		if (strchr(perm, 'w') != NULL) protection|=PROT_WRITE;
		if (strchr(perm, 'x') != NULL) protection|=PROT_EXEC;
	}

	if ((nodo->id = MmapFichero(arg[2], protection, nodo)) == NULL) {
		perror("Imposible mapear fichero");
		return 0;
	} else
		printf ("fichero %s mapeado en %p\n", arg[0], nodo->id);
	return 1;
}

void *ObtenerMemoriaMalloc(char* arg[], tNodo* nodo) {

	//((tDato*)nodo->dato)->extra = NULL;??
	((tDato *)nodo->dato)->size = strtoul(arg[2], NULL, 10);
	nodo->id = malloc(((tDato *)nodo->dato)->size);
	((tDato *)nodo->dato)->tipo = mallocc;

	return (*nodo).id;
}

void * ObtenerMemoriaShmget (key_t clave, off_t tam, tNodo* nodo) {
	void * p;
	int aux,id,flags=0777;
	struct shmid_ds s;

	if (tam) 	/*si tam no es 0 la crea en modo exclusivo */
		flags=flags | IPC_CREAT | IPC_EXCL;

	/*si tam es 0 intenta acceder a una ya creada*/
	if (clave==IPC_PRIVATE) { /*no nos vale*/
		errno=EINVAL;
		return NULL;
	}
	if ((id=shmget(clave,(size_t) tam, flags))==-1)
		return (NULL);
	if ((p=shmat(id,NULL,0))==(void*) -1){
		aux=errno;

		/*si se ha creado y no se puede mapear*/
		if (tam) /*se borra */
			shmctl(id,IPC_RMID,NULL);
		errno=aux;
		return (NULL);
	}
	shmctl (id,IPC_STAT,&s);
/* Guardar En Direcciones de Memoria Shared (p, s.shm_segsz, clave.....);*/
	((tDato *)nodo->dato)->size = s.shm_segsz;
	return (p);
}

int AllocCreateShared (char* arg[], tNodo *nodo) {
	key_t k;
	off_t tam = 0;
	// El parametro NULL es para pasar un string que devuelve un codigo de error
	// para el tratamiento de errores
	k = (key_t) strtol(arg[2],NULL,10);

	if (arg[3] != NULL) tam = (off_t) strtol(arg[3],NULL,10);

	if ((nodo->id = ObtenerMemoriaShmget(k,tam, nodo)) == NULL) //Se pasa nodo para conseguir el tamaño en caso de -shared
		perror ("Imposible obtener memoria shmget");
	else {
		printf("Memoria de shmeget de clave %d asignada en %p\n", k, nodo->id);
		((tShared* ) ((tDato*)nodo->dato)->extra)->key = k;
		return 1;
	}
	return 0;
}

void createNodo (tNodo* nodo, tType type) {
	time_t t;
	time(&t);
	nodo->dato = malloc(sizeof(tDato));
	((tDato *)nodo->dato)->extra = malloc(300); //sizeof(void *) da problemas con valgrind
	((tDato *)nodo->dato)->tipo = type;
	strcpy(((tDato *)nodo->dato)->date,asctime(localtime(&t)));
}

void freeNodo (tNodo* nodo) {
	if(((tDato*)nodo->dato)->extra != NULL )
		free((tType*)((tDato*)nodo->dato)->extra);
	free((tDato*)nodo->dato);
}

int cmd_allocate (container* c) {
	tNodo nodo;

	switch (c->nargs) {
		case 1:
			printList(-1, c->lista);
			return 0;
		case 2:
			if (!strcmp(c->flags[1], "-malloc")) printList(mallocc, c->lista);
			else if (!strcmp(c->flags[1], "-shared") || !strcmp(c->flags[1], "-createshared")) printList(shared, c->lista);
			else if (!strcmp(c->flags[1], "-mmap")) printList(mmapp, c->lista);
			else return COMANDO_INVALIDO;
			return 0;
		case 3:
			if(!strcmp(c->flags[1],"-malloc")) {
				createNodo(&nodo, mallocc);
				if (ObtenerMemoriaMalloc(c->flags, &nodo) != NULL) {
					if (!insertItem(nodo, NIL, &c->lista))
						return ERROR_INSERT;
					else
						printf("block at address %p allocated (malloc)\n", nodo.id);
				}
				else {
					freeNodo(&nodo);
					perror("cannot malloc");
				}
			}
			else if (!strcmp(c->flags[1], "-mmap")) {
				createNodo(&nodo, mmapp);
				if (cmd_mmap(c->flags, &nodo)) {
					if (!insertItem(nodo, NIL, &c->lista)) return ERROR_INSERT;
				}
				else freeNodo(&nodo);
			}
			else if (!strcmp(c->flags[1], "-shared")) {
				createNodo(&nodo, shared);
				if (AllocCreateShared(c->flags, &nodo)) {
					if (!insertItem(nodo, NIL, &c->lista))
						return ERROR_INSERT;
				}
				else freeNodo(&nodo);
			}
			else return COMANDO_INVALIDO;
			return 0;
		case 4:
			if (!strcmp(c->flags[1], "-mmap")) {
				createNodo(&nodo, mmapp);
				if (cmd_mmap(c->flags, &nodo)) {
					if (!insertItem(nodo, NIL, &c->lista)) return ERROR_INSERT;
				}
				else freeNodo(&nodo);
			}
			else if (!strcmp(c->flags[1], "-createshared")) {
				createNodo(&nodo, shared);
				if (AllocCreateShared(c->flags, &nodo)) {
					if (!insertItem(nodo, NIL, &c->lista)) return ERROR_INSERT;
				}
				else freeNodo(&nodo);
			}
			else return COMANDO_INVALIDO;
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

void deallocateAux (tList* l, tNodo nodo, tPosL pos, tType tipo) {
	int id;
	struct shmid_ds buf;

	switch (tipo) {
		case mallocc:
			printf("block at address %p deallocated (malloc)\n", nodo.id);
			free(nodo.id);
			freeNodo(&nodo);
			deleteAtPosition(pos, l);
			break;
		case mmapp:
			fsync(((tMap*)((tDato*) nodo.dato)->extra)->fd);
			if (!close(((tMap*)((tDato*) nodo.dato)->extra)->fd)) {
				printf("block at address %p deallocated (mmap)\n", nodo.id);
				munmap(nodo.id, ((tDato*) nodo.dato)->size);
				freeNodo(&nodo);
				deleteAtPosition(pos, l);
			}
			else printf("cannot unmap %s: %s\n",((tMap*)((tDato*) nodo.dato)->extra)->fichero, strerror(errno));
			break;
		case shared:
			printf("block at address %p deallocated (shared)\n", nodo.id);
			id = shmget(((tShared*)((tDato*)nodo.dato)->extra)->key, ((tDato*)nodo.dato)->size, 0);
			shmdt(nodo.id);
			shmctl (id,IPC_STAT,&buf);
			if (!buf.shm_nattch) //Si era la última correspondencia, se elimina la zona de memoria compartida
				shmctl(id, IPC_RMID, NULL);
			freeNodo(&nodo);
			deleteAtPosition(pos, l);
			break;
		default:
			break;
	}
}

int compare(tNodo nodo, void* p, tType type) {
	switch (type) {
		case mallocc: return ((tDato*)nodo.dato)->tipo == mallocc && ((tDato*)nodo.dato)->size == (((auxDealloc*) p)->tam);
		case mmapp: return ((tDato*)nodo.dato)->tipo == mmapp && !strcmp(((tMap*)((tDato*)nodo.dato)->extra)->fichero, ((auxDealloc*) p)->file);
		case shared: return ((tDato*)nodo.dato)->tipo == shared && ((tShared*)((tDato*)nodo.dato)->extra)->key == ((auxDealloc* ) p)->key;
		default: return 0;
	}
}

void searchDealloc (tType type, auxDealloc aux, tList* l) {
	int i, b = 0;
	i = first(*l);
	tNodo nodo = {};

	if (!isEmptyList(*l)) {
		while ((i != NIL) && !b) {
			nodo = getItem(i, *l);
			if (compare(nodo, &aux, type))
				b = 1;
			else
				i = next(i, *l);
		}
		if (b) {
			deallocateAux(l, nodo, i, type);
		}
		else
			printList(type, *l);
	}
}

int cmd_deallocate (container* c){
	tNodo nodo;
	tPosL i;
	auxDealloc aux = {};

	switch (c->nargs) {
		case 1:
			printList(-1, c->lista);
			break;
		case 2:
			if (!strcmp(c->flags[1], "-malloc")) printList(mallocc, c->lista);
			else if (!strcmp(c->flags[1], "-mmap")) printList(mmapp, c->lista);
			else if (!strcmp(c->flags[1], "-shared")) printList(shared, c->lista);
			else {
				i = findItem(c->flags[1], c->lista);
				if (i != NIL) {
					nodo = getItem(i, c->lista);
					deallocateAux(&c->lista, nodo, i, ((tDato*)nodo.dato)->tipo);
				}
				else
					printList(-1, c->lista);
			}
			break;
		case 3:
			if (!strcmp(c->flags[1], "-malloc")) {
				aux.tam = strtoul(c->flags[2], NULL, 10);
				searchDealloc(mallocc, aux, &c->lista);
			}
			else if (!strcmp(c->flags[1], "-mmap")){
				strcpy(aux.file, c->flags[2]);
				searchDealloc(mmapp, aux, &c->lista);
			}
			else if (!strcmp(c->flags[1], "-shared")) {
				aux.key = (int) strtoimax(c->flags[2], NULL, 10);
				searchDealloc(shared, aux, &c->lista);
			}
			else {
				return COMANDO_INVALIDO;
			}
			break;
		default:
			return COMANDO_INVALIDO;
	}

	return 0;
}

int cmd_rmkey (container* c) {
	key_t clave;
	int id;
	if (c->nargs == 2) {
		char *key = c->flags[1];

		if (key == NULL || (clave = (key_t) strtoul(key, NULL, 10)) == IPC_PRIVATE) {
			printf("rmkey clave_valida\n");
		}
		if ((id = shmget(clave, 0, 0666)) == -1) {
			perror("shmget: imposible obtener memoria compartida");
			return 0;
		}
		if (shmctl(id, IPC_RMID, NULL) == -1) {
			perror("shmctl: imposible eliminar memoria compartida");
			return 0;
		}

		return 0;
	}
	else return COMANDO_INVALIDO;
}

int cmd_mem (container* c){
	int local1;
	int local2;
	int local3;

	if (c->nargs == 1) {
		printf("Direcciones de memoria de tres funciones del programa:\n");
		printf("cmd_allocate -> %p\ncmd_deallocate -> %p\ncmd_autores -> %p\n\n", &cmd_allocate, &cmd_deallocate,
			   &cmd_autores);
		printf("Direcciones de memoria de variables globales:\n");
		printf("global1 -> %p\nglobal2 -> %p\nglobal3 -> %p\n\n", &global1, &global2, &global3);
		printf("Direcciones de memoria de variables locales:\n");
		printf("local1 -> %p\nlocal2 -> %p\nlocal3 -> %p\n", &local1, &local2, &local3);
		return 0;
	}
	else return COMANDO_INVALIDO;
}

int cmd_memDump (container* c){
	int cont, j;
	uintptr_t valor;
	uint32_t *puntero;
	char aux[25];

	switch (c->nargs) {
		case 2:
			c->flags[2] = "25";
		case 3:
			valor = strtoul(c->flags[1], NULL, 0);
			cont = (int) strtol(c->flags[2], NULL, 10);
			while (cont > 0) {
				puntero = (void *) valor;
				if (cont >= 25)
					memcpy(aux, puntero, 25);
				else
					memcpy(aux, puntero, (size_t)cont);

				for (j = 0; (j < 25) && (j < cont); j++) {
					if (isprint(aux[j]))
						printf("%2c ", aux[j]);
					else
						printf("   ");
				}
				printf("\n");
				for (j = 0; (j < 25) && (j < cont); j++) {
					if (isprint(aux[j]))
						printf("%x ", aux[j]);
					else
						printf("%x ", ' ');
				}
				printf("\n");
				valor = valor + 25;
				cont = cont - 25;
			}
			break;
		default:
			return COMANDO_INVALIDO;
	}

	return 0;
}

void auxRecursive (int n) {
	char automatico[1024];
	static char estatico[1024];

	if (n > 0) {
		printf("parametro n:%d en %p\n", n, &n);
		printf("array estatico en: %p \n", &estatico);
		printf("array automatico en: %p\n", &automatico);
		n--;
		auxRecursive(n);
	}
}

int cmd_recursiveFunction (container *c){

	if (c->nargs == 2) {
		auxRecursive((int) strtoimax(c->flags[1], NULL, 10));
	}
	else return COMANDO_INVALIDO;

	return 0;
}

#define LEERCOMPLETO ((ssize_t)-1)
ssize_t LeerFichero (char *fich, void *p, ssize_t n) { /*n=-1 indica que se lea t_odo */

	ssize_t nleidos, tam = n;
	int df, aux;
	struct stat s;

	if(stat (fich, &s)==-1||(df=open(fich, O_RDONLY))==-1)
		return ((ssize_t)-1);

	if (n==LEERCOMPLETO)
		tam=(ssize_t) s.st_size;

	if ((nleidos = read(df,p,(size_t)tam))==-1){
		aux = errno;
		close(df);
		errno = aux;
		return ((size_t)-1);
	}
	close (df);
	return nleidos;
}

int cmd_read (container *c){
	ssize_t cont = LEERCOMPLETO;
	uintptr_t valor;
	uint32_t *puntero;

	switch (c->nargs) {
		case 3:
		case 4:
			valor = strtoul(c->flags[2], NULL, 0);
			puntero = (void *) valor;
				if (c->nargs == 4)
					cont = strtoul(c->flags[3], NULL, 10);
				cont = LeerFichero(c->flags[1], puntero, cont);
				if (cont != -1)
					printf("Read %lu bytes from file %s into %p\n", cont, c->flags[1], puntero);
				else
					printf("cannot read %s: %s\n", c->flags[1], strerror(errno));
			break;
		default:
			return COMANDO_INVALIDO;
	}
	return 0;
}

int cmd_write (container *c){
	int fd;
	size_t cont;
	uintptr_t valor;
	uint32_t *puntero;
	FILE *fichero;

	switch (c->nargs) {
		case 4:
		case 5:
			if (access(c->flags[1], F_OK) || (c->flags[4] != NULL && !strcmp(c->flags[4],"-o"))) {
				valor = strtoul(c->flags[2], NULL, 0);
				puntero = (void *) valor;
				cont = strtoul(c->flags[3], NULL, 10);
				if ((fichero = fopen(c->flags[1], "w+")) != NULL) {
					fd = fileno(fichero);
					if (write(fd, puntero, cont) != -1) {
						printf("Written %lu bytes from memory address %p into file %s", cont, puntero, c->flags[1]);
					} else
						printf("cannot write %s: %s\n", c->flags[1], strerror(errno));
					fclose(fichero);
				} else
					printf("cannot write %s: %s\n", c->flags[1], strerror(errno));
			}
			else {
				if (c->nargs == 4)
					printf("File %s already exists, to overwrite it use -o\n", c->flags[1]);
				else
					return COMANDO_INVALIDO;
			}
			break;
		default:
			return COMANDO_INVALIDO;
	}
	return 0;
}

int cmd_setpriority (container *c) {
	id_t pid;
	int priority;
	errno = 0; //Necesario para procesamiento de error con getpriority

	switch (c->nargs) {
		case 2:
			pid = (id_t) strtoimax(c->flags[1], NULL, 10);
			priority = getpriority(PRIO_PROCESS, pid);
			if (!errno)
				printf("Priority of process %d: %d\n", pid, priority);
			else
				perror("Cannot get priority");
			break;
		case 3:
			pid = (id_t) strtoimax(c->flags[1], NULL, 10);
			priority = (int) strtoimax(c->flags[2], NULL, 10);
			if (setpriority(PRIO_PROCESS, pid, priority) == 0)
				printf("Priority of process %d changed to: %d\n", pid, priority);
			else
				perror("Cannot set priority");
			break;
		default:
			return COMANDO_INVALIDO;
	}
	return 0;
}

int cmd_fork(container *c) {
	int pid;

	if ((pid = fork()) == 0) {
	}
	else
		waitpid(pid, NULL, 0);

	return 0;
}

void addPathSearchList(tList *l){
	tNodo nodo;
	char *aux;
	char *p;

	if ((p=getenv("PATH"))==NULL){
		printf("Imposible obtener PATH del sistema\n");
		return;
	}

	aux=strdup(p);
	p=strtok(aux,":");

	nodo.id = p;

	if((p)!=NULL && !insertItem(nodo,last(*l),l))
		printf("Imposible anadir %s: %s\n", p, strerror(errno));

	while((p=strtok(NULL,":"))!=NULL) {
		nodo.id = p;
		if (!insertItem(nodo,last(*l),l)) {
			printf("Imposible anadir %s: %s\n", p, strerror(errno));
		}
	}

	free(aux);
}

char *searchExec(tList l, char ejec[]){
	static char aux[MAXNOMBRE];
	int i;
	struct stat s;

	if (ejec==NULL)
		return NULL;
	if (ejec[0]=='/' || !strncmp(ejec,"./",2) || !strncmp(ejec,"../",2)) {
		if(stat(ejec,&s)!=-1)
			return ejec;
		else
			return NULL;
	}
	for(i=0; l.Array[i].id != NULL; i++) {
		sprintf(aux, "%s/%s", (char *)l.Array[i].id, ejec);
		if(stat(aux,&s)!=-1)
			return aux;
	}
	return NULL;
}

int cmd_searchList(container *c){

	int i;
	char aux[256];
	tNodo nodo={};

	switch(c->nargs){
		case 1:
			if(!strcmp(c->flags[0],"searchlist"))
				for(i=0; i<(c->searchList.fin); i++)
					printf(" %s\n",((char*)c->searchList.Array[i].id));
			break;
		case 2:
			switch(c->flags[1][0]){
				case '-':
					if(!strcmp("-path",c->flags[1])){
						addPathSearchList(&c->searchList);
					} else {
						return COMANDO_INVALIDO;
					}
					break;
				case '+':
					strcpy(aux, &c->flags[1][1]);
					strcpy(nodo.id,aux);
					strcpy(nodo.dato,"");
					insertItem(nodo,last(c->searchList),&c->searchList);
					break;
				default:
					printf("%s", searchExec( c->lista, c->flags[1]));
					break;
			}
			break;
		default:
			return COMANDO_INVALIDO;
	}
	return 0;
}

int cmd_exec(container* c) {
	int prioridad;
	char *flagsExec[c->nargs];

	for (int i = 0; i < c->nargs-1; i++) {
		flagsExec[i] = malloc(256);
	}
	flagsExec[c->nargs-1] = NULL;

	if (c->nargs < 2)
		return COMANDO_INVALIDO;
	else {
		for (int i = 0; i < c->nargs-1; i++) {
			strcpy(flagsExec[i], c->flags[i + 1]);
		}

		if (c->flags[c->nargs - 1][0] == '@') {
			prioridad = (int) strtoimax(&c->flags[c->nargs - 1][1], NULL, 10);
			setpriority(PRIO_PROCESS, (id_t) getpid(), prioridad);
			flagsExec[c->nargs-2] = NULL;
		}

		flagsExec[0] = searchExec(c->searchList, c->flags[1]);

		if (flagsExec[0] == NULL)
			printf("Imposible ejecutar %s: No se ha encontrado el fichero\n", c->flags[1]);
		else if (execv(flagsExec[0], flagsExec) == -1) {
			perror("No se ha podido ejecutar el fichero");
		}
	}

	for (int i = 0; i < c->nargs; i++) {
		if (flagsExec[i] != NULL)
			free(flagsExec[i]);
	}

	return 0;
}

int cmd_prog(container *c){

	int PID;
	int i;
	int status;
	char *args[c->nargs -2];

	for(i=0; i<c->nargs-2; i++){
		args[i] =  c->flags[i+2];
	}

	switch(PID = fork()) {

		case -1:
			perror("fallo en fork");
			return ERROR_FORK;
		case 0:
			if(execv(c->flags[1],c->flags)==-1){
				perror("fallo en exec");
			}
			break;
		default:
			while (wait(&status) != PID);
			if(status == 0){
				printf("Ejecución normal del hijo\n");
			} else {
				printf("Error del hijo\n");
			}
			break;
	}

	return 0;
}

int cmd_background(container* c) {
	if (c->nargs < 2)
		return COMANDO_INVALIDO;
	else {

	}
	return 0;
}

void freeList(container *c){

	tPosL pos = first(c->lista);
	tNodo aux;

	while(!isEmptyList(c->lista)){
		if(pos != NIL){
			aux = getItem(pos,c->lista);
			deallocateAux(&c->lista, aux, pos, ((tDato *)aux.dato)->tipo);
		}
		pos = next(pos,c->lista);
	}

	free(c->lista.Array);
	free(c->searchList.Array);
	printf(" Bye !\n");
}

int cmd_exit(container *c) {
	freeList(c);
	return 1;
}

struct{
	char *CMD_NAME;
	int (*CMD_FUN)(container* c);
} CMDS[] = {
		{"autores",cmd_autores},
		{"pid", cmd_pid},
		{"chdir", cmd_chdir},
		{"fecha", cmd_fecha},
		{"hora", cmd_hora},
		{"create", cmd_create},
		{"delete", cmd_delete},
		{"query", cmd_query},
		{"list", cmd_list},
		{"allocate", cmd_allocate},
		{"deallocate", cmd_deallocate},
		{"rmkey", cmd_rmkey},
		{"mem", cmd_mem},
		{"memdump", cmd_memDump},
		{"recursivefunction", cmd_recursiveFunction},
		{"read", cmd_read},
		{"write", cmd_write},
		{"setpriority", cmd_setpriority},
		{"fork", cmd_fork},
		{"searchlist", cmd_searchList},
		{"exec", cmd_exec},
		{"prog", cmd_prog},
		{"exit",cmd_exit},
		{"end",cmd_exit},
		{"fin",cmd_exit},
		{NULL, NULL}
};

int cmdManager(container* c){
	int i;
	for(i = 0; CMDS[i].CMD_NAME != NULL; i++){
		if(!strcmp(CMDS[i].CMD_NAME,c->flags[0])){
			return  CMDS[i].CMD_FUN(c);
		}
	}
	return COMANDO_INVALIDO;
}

int procesarEntrada(char *cadena, container *c){

	if((c->nargs = trocearCadena(cadena, c->flags))){
		return cmdManager(c);
	}
	return 0;
}

int main() {

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File","ERROR Deleting File",
	                         "ERROR Deleting Directory","ERROR Listing","ERROR Inserting", "ERROR IPC",
	                         "ERROR Reading", "ERROR Fork"};
	clear();
	char *entrada ;
	container c;
	int salir = 0;
	entrada = malloc(1024);
	createEmptyList(&c.lista, MAXALLOC);
	createEmptyList(&c.searchList,MAXSEARCHLIST);


	while (salir<=0) {
		imprimirPrompt();
		leerEntrada(entrada);
		salir = procesarEntrada(entrada, &c);
		if(salir<0)
			printf("%s",ERROR_MESAGES[abs(salir)]);
	}
	free(entrada);

}