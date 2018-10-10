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

void imprimirPrompt(int counter){
	printf("\n $ ");
}

void leerEntrada( char * cadena){
	fgets(cadena, 1024, stdin);
}

int cmd_autores(char * flags[], int nargs){
	int i = 1;
	int valid = 1;

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
				valid = 0 ;;
				return COMANDO_INVALIDO;
			}
			printf("%s", salida);
			break;
		default :
			return COMANDO_INVALIDO;
	}
	/*if(nargs == 1){

		strcpy(salida, autores[0]);
		strcat(salida, "\t\t\t");
		strcat(salida, autores[1]);
		strcat(salida,"\n\t");
		strcat(salida, correos[0]);
		strcat(salida, "\t\t");
		strcat(salida, correos[1]);
		printf("%s",salida);
	} else if(nargs < 4){
			strcpy(salida,"");
			while(i < nargs && valid){
				if(!strcmp(flags[i],"-n")){
					strcat(salida, autores[0]);
					strcat(salida, "\t\t\t");
					strcat(salida, autores[1]);
				} else if(!strcmp(flags[i],"-l")) {
					strcat(salida, correos[0]);
					strcat(salida, "\t\t");
					strcat(salida, correos[1]);
				} else {
					valid = 0 ;;
					return COMANDO_INVALIDO;
				}
				i++;
				if(i<nargs) strcat(salida,"\n\t");
			}
			printf("%s", salida);
	} else {
		return COMANDO_INVALIDO;
	}*/
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

int cmd_fecha (char * flags[], int num) {
	struct tm *t;
	time_t s;

	time(&s);
	t = localtime(&s);

	switch (num) {
		case 1:
			printf(strtok(asctime(t),"\n"));
			return 0;
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_chdir(char * flags[], int nargs){
	char dir[1024];

	if(nargs == 1){
		getcwd(dir,1024);
		printf("%s", dir);
	} else if(nargs == 2){
		if(chdir(flags[1])!=0){
			sprintf(dir," chdir: %s", strerror(errno));
			printf("%s", dir);
		};
	} else {
		return COMANDO_INVALIDO;
	}

	return 0;
}

int cmd_create(char *flags[], int nargs) {
	FILE *fp;

	switch (nargs) {
		case 2:
			fp = fopen(flags[2],"w");
			printf("%p\t",fp);
			if ( fp==NULL ) { return ERROR_CREATING_FILE; }
			fclose ( fp );
			return 0;
		case 3:
			if(!strcmp(flags[1],"-d")){
				mkdir(flags[2],0777);
				return 0;
			}else{
				return COMANDO_INVALIDO;
			}
		default:
			return COMANDO_INVALIDO;
	}
}

int cmd_delete(char *flags[], int nargs) {

}

int cmd_query(char *flags[], int nargs) {

}

int cmd_list(char *flags[], int nargs) {

}

struct{
	char * CMD_NAME;
	int (*CMD_FUN)(char * trozos[], int nargs);
} CMDS[] = {
		{"autores",cmd_autores},
		{"pid", cmd_pid},
		{"chdir", cmd_chdir},
		{"fecha", cmd_fecha},
		{"create", cmd_create},
		{"delete", cmd_delete},
		{"query", cmd_query},
		{"list", cmd_list},
		{"exit", 1},
		{"quit", 1},
		{"close", 1},

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

int main() {

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido","ERROR Creating File"};

	clear();
	char * entrada ;
	int salir = 0;
	int counter = 1;
	entrada = malloc(1024);
	while ((salir<=0)) {
		imprimirPrompt(counter);
		leerEntrada(entrada);
		salir = procesarEntrada(entrada);
		if(salir<0)printf("%s",ERROR_MESAGES[abs(salir)]);
		counter++;
	}
	free(entrada);
}