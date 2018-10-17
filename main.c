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
//include <ftw.h>

#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2
#define ERROR_DELETING_FILE -3

#define clear() printf("\033[H\033[J")

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

char * ConvierteModo (mode_t m, char *permisos)
{
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
	return permisos;
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

char * ConvierteModo3 (mode_t m)
{
	char * permisos;
	permisos=(char *) malloc (12);
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
	char *salida = malloc(1024);

	switch(nargs){
		case 1 :
			strcpy(salida, autores[0]);
			strcat(salida, "\t\t\t");
			strcat(salida, autores[1]);
			strcat(salida,"\n\t");
			strcat(salida, correos[0]);
			strcat(salida, "\t\t");
			strcat(salida, correos[1]);
			printf("%s",salida);
			break;
		case 2 :
			if(!strcmp(flags[i],"-n")){
				strcpy(salida, autores[0]);
				strcat(salida, "\t\t\t");
				strcat(salida, autores[1]);
			} else if(!strcmp(flags[i],"-l")) {
				strcpy(salida, correos[0]);
				strcat(salida, "\t\t");
				strcat(salida, correos[1]);
			} else {
				return COMANDO_INVALIDO;
			}
			printf("%s", salida);
			break;
		default :
			return COMANDO_INVALIDO;
	}

	free(salida);
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
	FILE *fp;

	switch (nargs) {
		case 2:
			fp = fopen(flags[1],"w");
			if ( fp==NULL ) {
				fclose ( fp );
				return ERROR_CREATING_FILE;
			}
			fclose ( fp );
			return 0;
		case 3:
			if(!strcmp(flags[1],"-d")){
				if (!mkdir(flags[2],0777)) {
					return 0;
				}
				else {
					if (errno == 13) {
						printf("cannot create %s: permission denied\n", flags[2]);
					}
					else {
						printf("create: %s\n", strerror(errno));
					}
					return ERROR_CREATING_FILE;
				}
			}else{
				return COMANDO_INVALIDO;
			}
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_delete(char *flags[], int nargs) {
	/*
	 * TODO - Eliminar un fichero o un directorio
	 * DONE - Si no tiene flag el directorio tiene que estar vacio para ser borrado
	 * TODO - Si tiene un flag -r borrara todo el contenido y el directorio
	 * DONE - Si no hay argumentos no se hace nada
	 * TODO - Si no se puede eliminar Hay que informar al usuario con un mensaje
	 */
	struct stat path_stat;
	DIR *dir;
	struct dirent *ent;
	char *aux[1024];

	switch ( nargs ) {
		case 1:
			return 0;
		case 2:
			stat(flags[1],&path_stat);
			if (strcmp(flags[1],"-r")) {
				if (S_ISDIR(path_stat.st_mode)) {
					if (rmdir(flags[1])) {
						printf("cannot delete: %s\n", strerror(errno));
						return ERROR_DELETING_FILE;
					}
				}
				else {
					if (unlink(flags[1])) {
						printf("cannot delete: %s\n", strerror(errno));
						return ERROR_DELETING_FILE;
					}
				}
			}
			return 0;
		case 3:
			if (strcmp(flags[1], "-r")) {
				return COMANDO_INVALIDO;
			}
			else {
				for (int i=0; i<3;i++) {
					aux[i] = flags[i];
				}
				stat(flags[2],&path_stat);
				if (S_ISDIR(path_stat.st_mode)) {
					if ((dir = opendir(flags[2])) != NULL) {
						chdir(flags[2]);
						while ((ent = readdir(dir)) !=NULL) {
							aux[2] = ent->d_name;
							if (strcmp(aux[2],".") && strcmp(aux[2],"..")) {
								cmd_delete(aux,3);
							}
						}
						chdir("..");
						closedir(dir);
						aux[1] = flags[2];
						cmd_delete(aux, 2);
					}
				}
				else {
					aux[1] = flags[2];
					cmd_delete(aux, 2);
				}
			}
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_query(char *flags[], int nargs) {
	/*
	 * TODO - Devuelve informacion de los archivos/directorios pasados como argumentos
	 * TODO - Produce una linea por arch/dir pasado como argumento
	 * TODO - mismo formato que ls -li resolviendo links simbolicos si es necesario
	 * TODO - equivalente a ls -li para archivos y que ls -lid para directorios
	 */
	int i;
	struct stat fileStat;
	char * permisos;
	permisos = malloc(1024);

	for(i = 1; i<nargs; i++){

		if(stat(flags[i], &fileStat)==0){
			ConvierteModo(fileStat.st_mode,permisos);
			struct passwd *pw = getpwuid(fileStat.st_uid);
			struct group *gr = getgrgid(fileStat.st_gid);
			time_t t = fileStat.st_mtim.tv_sec;
			struct tm *tm = localtime(&t);
			char buf[200];

			strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", tm);
			printf("%ju ", fileStat.st_ino);
			printf("\t %s ", permisos);
			printf("\t %s ", pw->pw_name);
			printf("\t %s ", gr->gr_name);
			printf("\t %s ", fileStat.st_size);
			printf("\t %s ",buf);

			//printf("\t %s %d %d:%d ", tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);

		} else {
			printf("\n Error >> <<");
		}

	}

	free(permisos);
 	return 0;
}

int cmd_list(char *flags[], int nargs) {
	/*
	 * TODO - Lista los directorios y/o archivos proporcionados por los argumentos de linea de comandos
	 * TODO - (-n) Solo listara el nombre y el tamaño de cada fich y/o dir. En cualquier otro caso se listara exactamente igual que query
	 * TODO - (-r) Los directorios y su contenido seran listados de forma recusrsiva
	 * TODO - (-h) Los fich/dir que empiecen por . no seran listados a menos que se ponga -h
	 * TODO - Para averiguar el tipo de sistema de ficheros tendremos que usar una llamada al sistema, NO USAR EL CAMPO d_type
	 * TODO - Si no se proporciona ningun nombre se listara el directorio de trabajo actual
	 * opendir() -> para el listado recursivo
	 */
	struct stat path_stat;
	int index = 0, option = 0;
	int nflag = 0, hflag = 0, rflag = 0, sumf = 0;
	DIR *dir;
	struct dirent *ent;

	while ((index = getopt(nargs, flags, "nhr")) != -1) {
		switch (index) {
			case 'n':
				nflag = 1;
				break;
			case 'h':
				hflag = 2;
				break;
			case 'r':
				rflag = 5;
				break;
			default:
				break;
			}
		}
	sumf = nflag + hflag + rflag;

	switch (sumf) {
		case 0:
			break;
		case 1:
			if (nargs > 2) {
				for (int i = 2; i < nargs; i++) {
					if ((dir = opendir(".")) != NULL) {
						while ((ent = readdir(dir)) != NULL) {
							if (!strcmp(flags[i], ent->d_name)) {
								printf("%s \n", ent->d_name);
							}
						}
						closedir(dir);
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					while ((ent = readdir(dir)) != NULL) {
						printf("%s \n", ent->d_name);
					}
					closedir(dir);
				}
			}
			break;
		case 2:
			break;
		case 3:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		default:
			break;
	}

	/*if (flags[1]=="-n") {
		if (flags[2]=="-h") {
			if (flags[3] == "-r") {

			}
			else {

			}
		}
		else {
			if (flags[2]=="-r") {

			}
			else{

			}
		}
	}
	else {
		if (flags[1]=="-h") {
			if (flags[2] == "-r") {

			}
			else {

			}
		}
		else {
			if (flags[1] == "-r") {

			}
			else {

			}
		}
	}*/
	/*
	//Declaramos variables, estructuras
	struct stat estru;
	struct dirent *dt;
	DIR *dire;

	dire = opendir(nombre);

	printf("abriendo el directorio %s\n",nombre);
	//Recorrer directorio
	while((dt=readdir(dire))!=NULL){
		//strcmp permite comparar, si la comparación es verdadera devuelve un 0
		//Aquí se pregunta si el arhivo o directorio es distinto de . y ..
		//Para así asegurar que se muestre de forma recursiva los directorios y ficheros del directorio actual
		if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){
			stat(dt->d_name,&estru);
			//Si es un directorio, llamar a la misma función para mostrar archivos
			if(S_ISDIR(estru.st_mode)){
				lista_directorio(dt->d_name);
				//Si no es directorio, mostrar archivos
			}else{
				imprime_permisos(estru);
				printf("%-20s %d \n",dt->d_name,estru.st_size);
			}
		}

	}
	closedir(dire);

} */
	return 0;
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
		{"exit",cmd_exit},
		{"end",cmd_exit},
		{"fin",cmd_exit},

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
		printf("\t");
		return cmdManager(trozos,ntrozos);
	}
	return 0;
}
/*
 * TODO - Separar los comandos query y list en dos archivos query.c y list.c
 * TODO - Hacer que el programa pueda lidiar con caracteres tipo (*,?,...)
 * TODO - Cunado no se ejecuta el comando de la forma esperada hay que informar al usuarion con un mensaje de error, para cada comando
 * TODO - Revisar lo que comentan en la seccion HELPFUL INFORMATION
 */
int main() {

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File","ERROR Deleting File"};

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
