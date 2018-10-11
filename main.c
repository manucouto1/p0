#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2

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
	// TODO en el ejemplo que aparece en la practica saca un mensaje de error por permisos 
	// 	cuando no es capaz de crear un archivo o un directorio en algun path eso habria 
	// 	que mirarlo
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
				mkdir(flags[2],0777);
			}else{
				return COMANDO_INVALIDO;
			}
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_delete(char *flags[], int nargs) {
	/*
	 * TODO - Eliminar un fichero o un directorio
	 * TODO - Si no tiene flag el directorio tiene que estar vacio para ser borrado
	 * TODO - Si tiene un flag -r borrara todo el contenido y el directorio
	 * DONE - Si no hay argumentos no se hace nada
	 * TODO - Si no se puede eliminar Hay que informar al usuario con un mensaje
	 */
	switch ( nargs ) {
		case 1:
			// No aplica
			return 0;
		case 2:
			return 0;
		case 3:
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
	switch ( nargs ) {
		case 2:
			return 0;
		case 3:
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_list(char *flags[], int nargs) {
	/*
	 * TODO - Lista los directorios y/o archivos proporcionados por los argumentos de linea de comandos
	 * TODO - (-n) Solo listara el nombre y el tamaño de cada fich y/o dir. En cualquier otro caso se listara exactamente igual que query
	 * TODO - (-r) Los directorios y su contenido seran listados de forma recusrsiva
	 * TODO - (-h) Los fich/dir que empiecen por . no seran listados a menos que se ponga -h
	 * TODO - Para averiguar el tipo de sistema de ficheros tendremos que usar una llamada al sistema, NO USAR EL CAMPO d_type
	 * TODO - Si no se proporciona ningun nombre se listara el directorio de trabajo actual
	 */
	switch ( nargs ) {
		case 2:
			return 0;
		case 3:
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
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

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File"};

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
