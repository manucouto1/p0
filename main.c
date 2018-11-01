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
#include "list.h"
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2
#define ERROR_DELETING_FILE -3
#define ERROR_DELETING_DIR -4
#define ERROR_LISTING -5
#define ERROR_INSERT -6


#define clear() printf("\033[H\033[J")

typedef struct {
	tList lista;
	int nargs;
	char *flags[1024];
}container;

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
	int* key;
	size_t* tam;
	char* file;
} auxDealloc;

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


int cmd_autores(container* c){
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

void time_util(char * fecha[]){
	struct tm *t;
	time_t s;
	time(&s); //Guarda en s el tiempo en segundos desde 1/1/1970
	t = localtime(&s); //Convierte time_t a tm
	trocearCadena(asctime(t), fecha);
}

int cmd_hora (container* c){
	char * fecha[100];
	time_util(fecha);
	switch (c->nargs) {
		case 1 :
			printf("%s", fecha[3]);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_fecha (container *c) {
	char * fecha[100];
	time_util(fecha);
	switch (c->nargs) {
		case 1:
			printf("%s, %s/%s/%s", fecha[0], fecha[2], fecha[1], fecha[4]);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_chdir(container* c){
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

int cmd_exit(container *c) {
	return 1;
}

int cmd_create(container* c) {
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
				if (strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")) {
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

int cmd_delete(container *c) {
	/*
	 * DONE - Eliminar un fichero o un directorio
	 * DONE - Si no tiene flag el directorio tiene que estar vacio para ser borrado
	 * DONE - Si tiene un flag -r borrar el contenido y el directorio
	 * DONE - Si no hay argumentos no se hace nada
	 * DONE - Si no se puede eliminar Hay que informar al usuario con un mensaje
	 */

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
			if (strcmp(c->flags[1], "-r")) {
				return COMANDO_INVALIDO;
			} else {
				if (!stat(c->flags[2], &path_stat)) {
					borrar_rec(c->flags[2], path_stat);
				}
				else {
					printf("cannot delete %s: %s\n",c->flags[2], strerror(errno));
				}
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

		strcpy((*description).name, element);
		if (S_ISLNK(fileStat.st_mode)) {
			chdir(element);
			strcat((*description).name," -> ");
			strcat((*description).name, getcwd(fich, 1024));
			chdir(current);
		}
		strcpy((*description).permisos, ConvierteModo2(fileStat.st_mode));
		(*description).propietario = getpwuid(fileStat.st_uid)->pw_name;
		(*description).grupo = getgrgid(fileStat.st_gid)->gr_name;
		(*description).links = (long) fileStat.st_nlink;
		(*description).nInodo = fileStat.st_ino;
		time_t t = fileStat.st_mtim.tv_sec;
		tm = localtime(&t);

		strftime(buf, sizeof(buf), "%b %d %H:%M", tm);
		strcpy((*description).fecha, buf);

		fseek(file, 0L, SEEK_END);
		if (S_ISDIR(fileStat.st_mode) || S_ISLNK(fileStat.st_mode)) {
			(*description).size = fileStat.st_blksize;;
		}
		else {
			(*description).size = ftell(file);
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

int cmd_query(container* c) {
	/*
	 * DONE - Devuelve informacion de los archivos/directorios pasados como argumentos
	 * DONE - Produce una linea por arch/dir pasado como argumento
	 * DONE - mismo formato que ls -li resolviendo links simbolicos si es necesario
	 * DONE - equivalente a ls -li para archivos y que ls -lid para directorios
	 */

	int i;
	struct element_description description;

	for(i = 1; i<c->nargs; i++){
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

	if ((nivel<=1) || argR) {
		if (S_ISDIR(path_stat.st_mode)) {
			if ((dir = opendir(elemento)) != NULL) {
				nivel++;
				chdir(elemento);
				while ((ent = readdir(dir)) != NULL) {
					if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
						lstat(ent->d_name, &path_stat);
						if (!(!argH && (ent->d_name[0] == '.'))) {
							fun_list_rec(ent->d_name, path_stat, nivel, argH, argR, argN);
						}
					}
				}
				closedir(dir);
				if (!strcmp(elemento, "..")) {
					chdir(current);
				}
				else if (strcmp(elemento, ".")) chdir("..");
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

int cmd_list(container *c){
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
				}
				else {
					printf("cannot access %s: %s\n", c->flags[aux], strerror(errno));
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
	tPosL i = first(l);

	if (!isEmptyList(l)) {
		while ((i != NIL) && (((nodo = getItem(i, l)).tipo == type) || (type == -1))) {
			switch (nodo.tipo) {
				case mallocc:
					printf("%p: size:%lu malloc %s", nodo.addr, nodo.size, nodo.fecha);
					break;
				case mmapp:
					printf("%p: size:%lu mmap %s (fd: %d) %s", nodo.addr, nodo.size, ((mmap_info *) nodo.extra)->fich,
						   ((mmap_info *) nodo.extra)->fd, nodo.fecha);
					break;
				case shared:
					printf("%p: size:%lu shared memory (key: %d) %s", nodo.addr, nodo.size,
						   ((shared_info *) nodo.extra)->key, nodo.fecha);
					break;
				default:
					break;
			}
			i = next(i, l);
		}
	}
}

/*
 * TODO Allocate | reserva memoria y la guarda en la lista, si no se le pasan argumentos muestra los elementos de la lista
 * TODO -malloc [tam] | se le indica el tamaño devuelve la direccion de memoria, sin argumentos lista elementos
 * TODO -mmap fich [perm]
 * TODO -createshared [cl] [tam]
 * TODO -shared [cl]
 */
int cmd_allocate (container *c) {

	switch (c->nargs) {
		case 1:
			return COMANDO_INVALIDO;
		case 2:
			printList(-1, c->lista);
			break;
		case 3:
			if(!strcmp(c->flags[1],"-malloc")) {
				if(c->flags[2]!= NULL){
					size_t allocSize =(unsigned long) strtol(c->flags[2], NULL, 10);
					if(allocSize > 0){
						char *fecha[100];
						time_util(fecha);
						tNodo nodo;
						nodo.tipo = mallocc;
						nodo.size = allocSize;
						nodo.fecha = fecha[3];
						nodo.addr = malloc(allocSize);
						nodo.extra = NULL;
						if(!insertItem(nodo,NIL,&c->lista)) return ERROR_INSERT;

					}
				}
			}
			break;
		default:
			return COMANDO_INVALIDO;
	}

	return 0;
}

int compare(tNodo nodo, void* p, int num) {
	switch (num) {
		case 0: return (nodo.tipo == mallocc) && (nodo.size == *((size_t*) p));
		case 1: return (nodo.tipo == mmapp) && !strcmp(((mmap_info*) nodo.extra)->fich, (char*) p);
		case 2: return (nodo.tipo == shared) && ((shared_info*) nodo.extra)->key == *((int*) p);
		default: return 0;
	}
}

void deallocateAux (tList* l, tNodo nodo, tPosL pos, int num) {
	int id;
	void* p;

	switch (num) {
		case 0:
			printf("block at address %p deallocated (malloc)", nodo.addr);
			deleteAtPosition(pos, l);
			free(nodo.addr);
			break;
		case 1:
			fsync(((mmap_info*)nodo.extra)->fd);
			if (!close(((mmap_info*)nodo.extra)->fd)) {
				printf("block at address %p deallocated (mmap)", nodo.addr);
				munmap(nodo.addr, nodo.size);
				deleteAtPosition(pos, l);
			}
			else printf("cannot unmap %s: %s",((mmap_info* )nodo.extra)->fich, strerror(errno));
			break;
		case 2:
			printf("block at address %p deallocated (shared)", nodo.addr);
			id = shmget(((shared_info* )nodo.extra)->key, nodo.size, 0);
			p = shmat(id, nodo.addr, 0);
			shmdt(p);
			deleteAtPosition(pos, l);
			break;
		default:
			break;
	}
}

void searchDealloc (tType type, auxDealloc aux, tList* l) {
	int i, b = 0;
	i = first(*l);
	tNodo nodo;

	if (!isEmptyList(*l)) {
		while ((i != NIL) && !b) {
			nodo = getItem(i, *l);
			if (compare(nodo, aux.tam, type)) b = 1;
			else i = next(i, *l);
		}
		if (b) {
			deallocateAux(l, nodo, i, 0);
		} else printList(type, *l);
	}
	else printList(type, *l);
}
/*
 * TODO Deallocate | lobera una de las direcciones de memorias reservadas en la lista, sin argumentos lista las direcciones
 * TODO -malloc [tam] | Se elimina uno de los bloques de tamaño [tam] de la lista, si no hay o no se pasa argumento lista
 * TODO -mmap fich | deshace un mapeo del fichero <-> memoria y borra de lista,
 */
int cmd_deallocate (container* c){
	tNodo nodo;
	tPosL i;
	int b = 0;
	auxDealloc aux;

	aux.key = malloc(sizeof(int));
	aux.tam = malloc(sizeof(unsigned long));
	aux.file = malloc(sizeof(char)*200);

	switch (c->nargs) {
		case 1:
			printList(-1, c->lista);
			break;
		case 2:
			if (!strcmp(c->flags[1], "-malloc")) printList(mallocc, c->lista);
			else if (!strcmp(c->flags[1], "-mmap")) printList(mmapp, c->lista);
			else if (!strcmp(c->flags[1], "-shared")) printList(shared, c->lista);
			else {
				i = findItem((void* ) c->flags[1], c->lista);
				if (i != NIL) {
					nodo = getItem(i, c->lista);
					deallocateAux(&c->lista, nodo, i, nodo.tipo);
				}
				else printList(-1, c->lista);
			}
			break;
		case 3:
			if (!strcmp(c->flags[1], "-malloc")) {
				*aux.tam = (unsigned long) strtol(c->flags[2], NULL, 10);
				searchDealloc(mallocc, aux, &c->lista);
			}
			else if (!strcmp(c->flags[1], "-mmap")){
				strcpy(aux.file, c->flags[2]);
				searchDealloc(mmapp, aux, &c->lista);
			}
			else if (!strcmp(c->flags[1], "-shared")) {
				*aux.key = (int) strtoimax(c->flags[2], NULL, 10);
				searchDealloc(shared, aux, &c->lista);
			}
			else {
				free(aux.key);
				free(aux.file);
				free(aux.tam);
				return COMANDO_INVALIDO;
			}
			break;
		default:
			free(aux.key);
			free(aux.file);
			free(aux.tam);
			return COMANDO_INVALIDO;
	}

	free(aux.key);
	free(aux.file);
	free(aux.tam);
	return 0;
}
/*
 *TODO rmkey cl | Elimina la región de memoria compartida de llave cl. Simplemente es una llamada a shmctl(id, IPC RMID ...)
 */
int cmd_rmkey (container* c){

}
/*
 *TODO mem | Imprime la dirección de memoria de 3 funciones del programa, tres variables globales y tres variables locales
 */
int cmd_mem (container* c){

}
/*
 * TODO memdump addr | Enseña el contenido de 25 bytes empezando por addr
 * TODO memdump addr [cont] | Enseña el contenido cont bytes empezando por addr
 * Imprime 25 bytes por línea, podría producir segmentation fault
 */
int cmd_memDump (container* c){

}
/*
 *
 */
void auxRecursive (int n) {
	char automatico[1024];
	static char estatico[1024];

	printf ("parametro n:%d en %p\n",n,&n);
	printf ("array estatico en: %p \n",estatico);
	printf ("array automatico en: %p\n",automatico);
	n--;

	if (n>0) auxRecursive(n);
};
int cmd_recursiveFunction (container* c){

	if (c->nargs == 2) {
		auxRecursive((int) strtoimax(c->flags[1], NULL, 10));
	}
	else return COMANDO_INVALIDO;

	return 0;
}

/*
 * TODO read fich addr | Lee fich y guarda el resultado en addr (usando sólo una llamada read al sistema)
 * TODO read fich addr cont | Lee cont bytes de fich y guarda el resultado en addr (usando sólo una llamada read al sistema)
 */
int cmd_read (container* c){

}

/*
 * TODO write file addr cont [-o] | Escribe cont bytes de addr en file. Si file no existe se crea.
 * TODO Si ya existe, no se sobreescribe a menos que se le pase -o
 */
int cmd_write (container* c){

}

struct{
	char * CMD_NAME;
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

int procesarEntrada(char * cadena, container *c){

	if((c->nargs = trocearCadena(cadena, c->flags))){
		return cmdManager(c);
	}
	return 0;
}
/*
 * DONE - Separar los comandos query y list en dos archivos query.c y list.c
 * DONE - Hacer que el programa pueda lidiar con caracteres tipo (*,?,...)
 * DONE - Cunado no se ejecuta el comando de la forma esperada hay que informar al usuarion con un mensaje de error, para cada comando
 * DONE - Revisar lo que comentan en la seccion HELPFUL INFORMATION
 */
int main() {

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File","ERROR Deleting File", "ERROR Deleting Directory","ERROR Listing", "ERROR Inserting"};
	tList l;

	clear();
	char * entrada ;
	container c;
	int salir = 0;
	entrada = malloc(1024);
	createEmptyList(&l);
	c.lista = l;

	while ((salir<=0)) {
		imprimirPrompt();
		leerEntrada(entrada);
		salir = procesarEntrada(entrada, &c);
		if(salir<0)printf("%s",ERROR_MESAGES[abs(salir)]);
	}
	free(entrada);
}
