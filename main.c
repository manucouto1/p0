#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

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


	if(nargs == 1){

		strcpy(salida, autores[0]);
		strcat(salida, "\t\t\t");
		strcat(salida, autores[1]);
		strcat(salida,"\n\t");
		strcat(salida, correos[0]);
		strcat(salida, "\t\t");
		strcat(salida, correos[1]);
		//strcat(salida,"\n\t");
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
				printf("ERROR invalid command");
			}
			i++;
			if(i<nargs) strcat(salida,"\n\t");
		}
		printf("%s", salida);
	} else {
		printf("ERROR invalid command");
	}
	free(salida);
	return 0;

}

int cmd_pid(char * flags[], int num) {
	switch (num) {
		case 1:
			printf("Process ID: %d\n", getpid());
			return 1;
			break;
		case 2:
			if (!strcmp(flags[1],"-p")) {
				printf("PPID: %d\n", getppid());
				return 1;
			}
			else {
				printf("Error\n");
				return 0;
			}
			break;
		default:
			printf("Error\n");
			return 0;
	}
}

int cmd_fecha (char * flags[], int num) {
	struct tm *t;
	time_t s;

	time(&s);
	t = localtime(&s);

	switch (num) {
		case 1:
			printf(asctime(t));
			return 1;
			break;
		default:
			printf("Error\n");
			return 0;
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
		printf("ERROR invalid command !");
	}

	return 0;
}

struct{
	char * CMD_NAME;
	int (*CMD_FUN)(char * trozos[], int nargs);
} CMDS[] = {
		{"autores",cmd_autores},
		{"pid", cmd_pid},
		{"chdir", cmd_chdir},
		{"exit",1},
		{"quit",1},
		{"close", 1},
		{"fecha", cmd_fecha},
		{NULL, NULL}
};

int cmdManager(char *trozos[], int ntrozos){
	int i;
	for(i = 0; CMDS[i].CMD_NAME != NULL; i++){
		if(!strcmp(CMDS[i].CMD_NAME,trozos[0])){
			return  CMDS[i].CMD_FUN(trozos, ntrozos);
		}
	}
	printf("\tERROR invalid Command ");
	return 0;

}



int procesarEntrada(char * cadena){

	char * trozos [1024];
	int ntrozos;

	if((ntrozos = trocearCadena(cadena, trozos))){
		printf("\t");
		return cmdManager(trozos,ntrozos);
		//printf("\n");
	}
	return 0;
}

int main() {
	clear();
	char * entrada ;
	int salir = 0;
	int counter = 1;
	entrada = malloc(1024);
	while (!salir) {
		imprimirPrompt(counter);
		leerEntrada(entrada);
		salir = procesarEntrada(entrada);
		counter++;
	}
	free(entrada);
}