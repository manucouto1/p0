#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define COMANDO_INVALIDO -1

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

	char *ERROR_MESAGES[] = {"","ERROR Comando Invalido"};

	clear();
	char * entrada ;
	int salir = 0;
	int counter = 1;
	entrada = malloc(1024);
	while ((salir<=0)) {
		imprimirPrompt(counter);
		leerEntrada(entrada);
		salir = procesarEntrada(entrada);
		if(salir<0)printf("\t%s",ERROR_MESAGES[abs(salir)]);
		counter++;
	}
	free(entrada);
}