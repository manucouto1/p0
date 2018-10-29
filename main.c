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
//include <ftw.h>

#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2
#define ERROR_DELETING_FILE -3
#define ERROR_DELETING_DIR -4
#define ERROR_LISTING -5

#define clear() printf("\033[H\033[J")

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


int cmd_autores(char * flags[], int nargs){
	int i = 1;

	char *autores[2] = {"Manuel Couto Pintos","Victor Escudero Gonzalez"};
	char *correos[2] = {"manuel.couto1@udc.es", "victor.escudero@udc.es"};

	switch(nargs){
		case 1 :
			printf("%s\t%s\n", autores[0], autores[1]);
			printf("%s\t%s\n", correos[0], correos[1]);
			break;
		case 2 :
			if(!strcmp(flags[i],"-n")){
				printf("%s\t%s\n", autores[0], autores[1]);
			} else if(!strcmp(flags[i],"-l")) {
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

int cmd_pid(char * flags[], int num) {
	switch (num) {
		case 1:
			printf("Process ID: %d", getpid());
			return 0;
		case 2:
			if (!strcmp(flags[1],"-p")) {
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

int cmd_hora (char * flags[], int num){
	char * fecha[100];
	time_util(fecha);
	switch (num) {
		case 1 :
			printf("%s", fecha[3]);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_fecha (char * flags[], int num) {
	char * fecha[100];
	time_util(fecha);
	switch (num) {
		case 1:
			printf("%s, %s/%s/%s", fecha[0], fecha[2], fecha[1], fecha[4]);
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_chdir(char * flags[], int nargs){
	char dir[1024];

	switch ( nargs ) {
		case 1:
			getcwd(dir,1024);
			printf("%s",dir);
			return 0;
		case 2:

			if(chdir(flags[1])!=0){
				sprintf(dir," chdir: %s", strerror(errno));
				printf("%s", dir);
			};
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_exit(char * flags[], int nargs) {
	return 1;
}

int cmd_create(char *flags[], int nargs) {
	int fd;

	switch (nargs) {
		case 2:
			fd = open(flags[1], O_WRONLY | O_APPEND | O_CREAT, 0644);
			close(fd);
			return 0;
		case 3:
			if(!strcmp(flags[1],"-d")){
				if(!mkdir(flags[2],0777)){
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

int cmd_delete(char *flags[], int nargs) {
	/*
	 * DONE - Eliminar un fichero o un directorio
	 * DONE - Si no tiene flag el directorio tiene que estar vacio para ser borrado
	 * DONE - Si tiene un flag -r borrar el contenido y el directorio
	 * DONE - Si no hay argumentos no se hace nada
	 * DONE - Si no se puede eliminar Hay que informar al usuario con un mensaje
	 */

	struct stat path_stat;

	switch ( nargs ) {
		case 1:
			return 0;
		case 2:
			if (!stat(flags[1], &path_stat)) {
				if (S_ISDIR(path_stat.st_mode)) {
					return borrar_directorio(flags[1]);
				} else {
					return borrar_archivo(flags[1]);
				}
			}
			else {
				printf("cannot delete %s: %s\n",flags[1], strerror(errno));
			}
		case 3:
			if (strcmp(flags[1], "-r")) {
				return COMANDO_INVALIDO;
			} else {
				if (!stat(flags[2], &path_stat)) {
					borrar_rec(flags[2], path_stat);
				}
				else {
					printf("cannot delete %s: %s\n",flags[2], strerror(errno));
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

int cmd_query(char *flags[], int nargs) {
	/*
	 * DONE - Devuelve informacion de los archivos/directorios pasados como argumentos
	 * DONE - Produce una linea por arch/dir pasado como argumento
	 * DONE - mismo formato que ls -li resolviendo links simbolicos si es necesario
	 * DONE - equivalente a ls -li para archivos y que ls -lid para directorios
	 */

	int i;
	struct element_description description;

	for(i = 1; i<nargs; i++){
		if (load_data(flags[i], &description)) {
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

int cmd_list(char *flags[], int nargs){
	int i=1;
	int nCount=0;
	int rCount=0;
	int hCount=0;
	int nivel;
	struct stat path_stat;


	nivel = 0;

	while(i < nargs && (flags[i][0] == '-')){

		if(!strcmp(flags[i],"-n")){
			nCount++;
		} else if(!strcmp(flags[i],"-r")){
			rCount++;
		} else if(!strcmp(flags[i],"-h")){
			hCount++;
		} else {
			return COMANDO_INVALIDO;
		}
		i++;
	}

	int aux = hCount + rCount + nCount +1;

	if((nCount <= 1)&&(rCount <= 1)&& (hCount <= 1)){ // si solo hay flags y no hay argumento Error comando invalido
		//

		if(nargs!=aux){
			// Si se pasan argumentos
			for(aux = aux; aux < nargs; aux++){
				if (!lstat(flags[aux], &path_stat)) {
					fun_list_rec(flags[aux], path_stat, nivel, hCount, rCount, nCount);
				}
				else {
					printf("cannot access %s: %s\n", flags[aux], strerror(errno));
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
/*
 * TODO Allocate | reserva memoria y la guarda en la lista, si no se le pasan argumentos muestra los elementos de la lista
 * TODO -malloc [tam] | se le indica el tamaño devuelve la direccion de memoria, sin argumentos lista elementos
 * TODO -mmap fich [perm]
 * TODO -createshared [cl] [tam]
 * TODO -shared [cl]
 */
int cmd_allocate (char *flags[], int nargs) {

}

/*
 * TODO Delocate | lobera una de las direcciones de memorias reservadas en la lista, sin argumentos lista las direcciones
 * TODO -malloc [tam] | Se elimina uno de los bloques de tamaño [tam] de la lista, si no hay o no se pasa argumento lista
 * TODO -mmap fich | deshace un mapeo del fichero <-> memoria y borra de lista,
 */
int cmd_dealocate (char *flags[], int nargs){

}

/*
 *
 */
int cmd_rmkey (char *flags[], int nargs){

}

/*
 *
 *
 */
int cmd_mem (char *flags[], int nargs){

}

int cmd_memDump (char *flags[], int nargs){

}

int cmd_recursiveFunction (char *flags[], int nargs){

}

int cmd_read (char *flags[], int nargs){

}

int cmd_write (char *flags[], int nargs){

}

struct{
	char * CMD_NAME;
	int (*CMD_FUN)(char * trozos[], int nargs);
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
		{"deallocate", cmd_dealocate},
		{"rmkey", cmd_rmkey},
		{"mem", cmd_mem},
		{"memdump", cmd_memDump},
		{"recursivefunction", cmd_recursiveFunction},
		{"read", cmd_read},
		{"write", cmd_write},
		{"exit",cmd_exit},
		{"end",cmd_exit},
		{"fin",cmd_exit},

		{NULL, NULL},

		{NULL, NULL}
};

int cmdManager(char *trozos[], int ntrozos){
	int i;
	for(i = 0; CMDS[i].CMD_NAME != NULL; i++){
		if(!strcmp(CMDS[i].CMD_NAME,trozos[0])){
			return  CMDS[i].CMD_FUN(trozos, ntrozos);
		}
	}
	return COMANDO_INVALIDO;
}

int procesarEntrada(char * cadena){

	char * trozos [1024];
	int ntrozos;

	if((ntrozos = trocearCadena(cadena, trozos))){
		return cmdManager(trozos,ntrozos);
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

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File","ERROR Deleting File", "ERROR Deleting Directory","ERROR Listing"};

	clear();
	char * entrada ;
	int salir = 0;
	entrada = malloc(1024);
	while ((salir<=0)) {
		imprimirPrompt();
		leerEntrada(entrada);
		salir = procesarEntrada(entrada);
		if(salir<0)printf("%s",ERROR_MESAGES[abs(salir)]);
	}
	free(entrada);
}
