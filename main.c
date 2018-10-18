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
#include <sys/fcntl.h>
//include <ftw.h>

#define COMANDO_INVALIDO -1
#define ERROR_CREATING_FILE -2
#define ERROR_DELETING_FILE -3
#define ERROR_DELETING_DIR -4
#define ERROR_LISTING -5

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
		printf("\tcannot delete file %s: %s\n", elemento, strerror(errno));
		return ERROR_DELETING_FILE;
	} else {
		printf("\n\t Borrando archivo %s",elemento);
		return 0;
	}
}

int borrar_directorio(char * elemento){
	if (rmdir(elemento)) {
		printf("cannot delete directory %s: %s\n", elemento, strerror(errno));
		return ERROR_DELETING_DIR;
	} else {
		printf("\n\t Borrando directorio %s",elemento);
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
			stat(flags[1], &path_stat);
			if (S_ISDIR(path_stat.st_mode)) {
				return borrar_directorio(flags[1]);
			} else {
				return borrar_archivo(flags[1]);
			}
		case 3:
			if (strcmp(flags[1], "-r")) {
				return COMANDO_INVALIDO;
			} else {
				stat(flags[2], &path_stat);
				borrar_rec(flags[2], path_stat);
			}
			return 0;
		default:
			return COMANDO_INVALIDO;
	}

}

/* - LIST/QUERY util functions */
int print_li(char *element, char *permisos){

	struct stat fileStat;
	struct passwd *pw;
	struct group *gr;
	struct tm *tm;
	FILE *file;
	char buf[200];

	if( (stat(element, &fileStat)==0)&&((file=fopen(element,"r"))!=NULL)){
		permisos = ConvierteModo2(fileStat.st_mode);
		pw = getpwuid(fileStat.st_uid);
		gr = getgrgid(fileStat.st_gid);
		time_t t = fileStat.st_mtim.tv_sec;
		tm = localtime(&t);

		fseek(file, 0L, SEEK_END);
		strftime(buf, sizeof(buf), "%b %d %H:%M", tm);

		printf("%ju ", fileStat.st_ino);
		printf("%-5s ", permisos);
		printf("%-1ld ", (long) fileStat.st_nlink);
		printf("%-5s ", pw->pw_name);
		printf("%-5s ", gr->gr_name);
		printf("%ld ", ftell(file));
		printf("%-5s ",buf);
		printf("%s \n", element);
		//printf("\t %s %d %d:%d ", tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);

	} else {
		printf("Error >>%s<<\n", strerror(errno));
	}
}
/* - end LIST/QUERY util functions */

int cmd_query(char *flags[], int nargs) {
	/*
	 * DONE - Devuelve informacion de los archivos/directorios pasados como argumentos
	 * DONE - Produce una linea por arch/dir pasado como argumento
	 * TODO - mismo formato que ls -li resolviendo links simbolicos si es necesario
	 * DONE - equivalente a ls -li para archivos y que ls -lid para directorios
	 */

	int i;
	char * permisos;
	permisos = malloc(1024);

	for(i = 1; i<nargs; i++){
		print_li(flags[i], permisos);
	}

	free(permisos);
 	return 0;
}

void deleteAtPosition (char* flags[], int* nargs,int pos) {
	int i;
	for (i = pos; i<*nargs-1; i++) {
		flags [i] = flags [i+1];
	}
	*nargs = *nargs - 1;
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
	int index = 0, acu = 0;
	int i;
	int nflag = 0, hflag = 0, rflag = 0, sumf = 0;
	DIR *dir;
	struct dirent *ent;
	char *aux[1024]={flags[0]};
	while ((index = getopt(nargs, flags, "nhr")) != -1) {
		switch (index) {
			case 'n':
				switch (optind) {
					case 2:
						nflag = 1;
						break;
					case 3:
						if (rflag || hflag) {
							nflag = 1;
						}
						break;
					case 4:
						if (rflag && hflag) {
							nflag = 1;
						}
						break;
					default:
						break;
				}
				break;
			case 'h':
				switch (optind) {
					case 2:
						hflag = 2;
						break;
					case 3:
						if (rflag || nflag) {
							hflag = 2;
						}
						break;
					case 4:
						if (rflag && nflag) {
							hflag = 2;
						}
						break;
					default:
						break;
				}
				break;
			case 'r':
				switch (optind) {
					case 2:
						rflag = 5;
						break;
					case 3:
						if (hflag || nflag) {
							rflag = 5;
						}
						break;
					case 4:
						if (hflag && nflag) {
							rflag = 5;
						}
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	optind = 1;
	sumf = nflag + hflag + rflag;
	switch (sumf) {
		case 0:
			if (nargs>1) {
				for (i = 1; i< nargs; i++) {
					if (flags[i][0] == '.') {
						deleteAtPosition(flags,&nargs, i);
						i--;
					}
				}
				for (i = 1; i<nargs; i++) {
					if (!stat(flags[i],&path_stat)) {
						if (flags[i][0] != '.') {
							if ((dir = opendir(".")) != NULL) {
								while ((ent = readdir(dir)) != NULL) {
									if (!strcmp(flags[i], ent->d_name)) {
										stat(flags[i], &path_stat);
										aux[1] = ent->d_name;
										cmd_query(aux, 2);
										if (S_ISDIR(path_stat.st_mode)) {
											chdir(flags[i]);
											if ((dir = opendir(".")) != NULL) {
												while ((ent = readdir(dir)) != NULL) {
													if (ent->d_name[0] != '.') {
														aux[1] = strdup(ent->d_name);
														cmd_query(aux, 2);
													}
												}
												closedir(dir);
											}
											chdir("..");
										}
									}
								}
								closedir(dir);
							} else {
								return ERROR_LISTING;
							}
						} else {
							deleteAtPosition(flags, &nargs, i);
							i--;
						}
					}
					else {
						printf("cannot access %s: %s\n",flags[i], strerror(errno));
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					while ((ent = readdir(dir)) != NULL) {
						aux[1] = strdup(ent->d_name);
						cmd_list(aux, 2);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 1:
			if (nargs > 2) {
				for (i = 2; i < nargs; i++) {
					if (!stat(flags[i], &path_stat)) {
						if ((dir = opendir(".")) != NULL) {
							while ((ent = readdir(dir)) != NULL) {
								if ((!strcmp(flags[i], ent->d_name)) && (ent->d_name[0] != '.')) {
									printf("%s %ld\n", ent->d_name, path_stat.st_size);
									stat(flags[i], &path_stat);
									if (S_ISDIR(path_stat.st_mode)) {
										chdir(flags[i]);
										if ((dir=opendir(".")) != NULL) {
											while ((ent = readdir(dir)) != NULL) {
												if (ent->d_name[0] != '.') {
													stat(ent->d_name, &path_stat);
													printf("%s %ld\n", ent->d_name, path_stat.st_size);
												}
											}
											closedir(dir);
										}
										else {
											return ERROR_LISTING;
										}
										chdir("..");
									}
								}
							}
							closedir(dir);
						}
						else {
							return ERROR_LISTING;
						}
					}
					else {
						printf("cannot access %s: %s\n",flags[i], strerror(errno));
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-n");
					while ((ent = readdir(dir)) != NULL) {
						aux[2] = strdup(ent->d_name);
						cmd_list(aux, 3);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 2:
			if (nargs>2) {
				for (i = 2; i < nargs; i++) {
					if (!stat(flags[i], &path_stat)) {
						if ((dir = opendir(".")) != NULL) {
							while ((ent = readdir(dir)) != NULL) {
								if (!strcmp(flags[i], ent->d_name)) {
									aux[1] = strdup(ent->d_name);
									cmd_query(aux,2);
									stat(ent->d_name, &path_stat);
									if (S_ISDIR(path_stat.st_mode)) {
										if (strcmp(ent->d_name,".") && strcmp(ent->d_name, "..")) {
											chdir(ent->d_name);
											if ((dir = opendir(".")) != NULL) {
												while ((ent = readdir(dir)) != NULL) {
													aux[1] = strdup(ent->d_name);
													cmd_query(aux, 2);
												}
												closedir(dir);
											} else {
												return ERROR_LISTING;
											}
											chdir("..");
										}
									}
								}
							}
							closedir(dir);
						}
						else {
							return ERROR_LISTING;
						}
					}
					else {
						printf("cannot access %s: %s\n",flags[i], strerror(errno));
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-h");
					while ((ent = readdir(dir)) != NULL) {
						aux[2] = strdup(ent->d_name);
						cmd_list(aux, 3);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 3:
			if (nargs > 3) {
				for (i = 3; i < nargs; i++) {
					if (!stat(flags[i], &path_stat)) {
						if ((dir = opendir(".")) != NULL) {
							while ((ent = readdir(dir)) != NULL) {
								if (!strcmp(flags[i], ent->d_name)) {
									printf("%s %ld\n", ent->d_name, path_stat.st_size);
									stat(ent->d_name, &path_stat);
									if (S_ISDIR(path_stat.st_mode)) {
										if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
											chdir(ent->d_name);
											if ((dir = opendir(".")) != NULL) {
												while ((ent = readdir(dir)) != NULL) {
													printf("%s %ld\n", ent->d_name, path_stat.st_size);
												}
												closedir(dir);
											} else {
												return ERROR_LISTING;
											}
											chdir("..");
										}
									}
								}
							}
							closedir(dir);
						}
						else {
							return ERROR_LISTING;
						}
					}
					else {
						printf("cannot access %s: %s\n",flags[i], strerror(errno));
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-h");
					aux[2] = strdup("-n");
					while ((ent = readdir(dir)) != NULL) {
						aux[3] = strdup(ent->d_name);
						cmd_list(aux, 4);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 5:
			//printf("1");
			if (nargs > 2) {
				//printf("%d\n", nargs);
				for (i = 2; i<nargs; i++ ) {
					if ((dir = opendir(".")) != NULL) {
						if (!stat(flags[i], &path_stat)) {
							while ((ent = readdir(dir)) != NULL) {
								//printf("%s --- %s\n", ent->d_name, flags[i]);
								if ((!strcmp(ent->d_name, flags[i])) && (ent->d_name[0] != '.')) {
									//printf("1\n");
									aux[1] = strdup(ent->d_name);
									//printf("%s\n", aux[1]);
									cmd_query(aux, 2);
									stat(ent->d_name, &path_stat);
									if (S_ISDIR(path_stat.st_mode)) {
										chdir(ent->d_name);
										aux[1] = strdup("-r");
										cmd_list(aux, 2);
										chdir("..");
									}
								}
							}
						} else {
							printf("cannot access %s: %s\n", flags[i], strerror(errno));
						}
						closedir(dir);
					}
					else {
						return ERROR_LISTING;
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-r");
					while ((ent = readdir(dir)) != NULL) {
						aux[2] = strdup(ent->d_name);
						cmd_list(aux, 3);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 6:
			if (nargs > 3) {
				for (i = 3; i<nargs; i++ ) {
					if ((dir = opendir(".")) != NULL) {
						if (!stat(flags[i], &path_stat)) {
							while ((ent = readdir(dir)) != NULL) {
								if ((!strcmp(ent->d_name, flags[i])) && (ent->d_name[0] != '.')) {
									stat(ent->d_name, &path_stat);
									printf("%s %ld\n", ent->d_name, path_stat.st_size);
									if (S_ISDIR(path_stat.st_mode)) {
										chdir(ent->d_name);
										aux[1] = strdup("-r");
										aux[2] = strdup("-n");
										cmd_list(aux, 3);
										chdir("..");
									}
								}
							}
						} else {
							printf("cannot access %s: %s\n", flags[i], strerror(errno));
						}
						closedir(dir);
					}
					else {
						return ERROR_LISTING;
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-r");
					aux[2] = strdup("-n");
					while ((ent = readdir(dir)) != NULL) {
						aux[3] = strdup(ent->d_name);
						cmd_list(aux, 4);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 7:
			if (nargs > 3) {
				for (i = 3; i<nargs; i++ ) {
					if ((dir = opendir(".")) != NULL) {
						if (!stat(flags[i], &path_stat)) {
							while ((ent = readdir(dir)) != NULL) {
								if (!strcmp(ent->d_name, flags[i])) {
									aux[1] = ent->d_name;
									cmd_query(aux, 2);
									stat(ent->d_name, &path_stat);
									if (S_ISDIR(path_stat.st_mode)) {
										if (strcmp(ent->d_name,".") && strcmp(ent->d_name, "..")) {
											chdir(ent->d_name);
											aux[1] = strdup("-r");
											aux[2] = strdup("-h");
											cmd_list(aux, 3);
											chdir("..");
										}
									}
								}
							}
						} else {
							printf("cannot access %s: %s\n", flags[i], strerror(errno));
						}
						closedir(dir);
					}
					else {
						return ERROR_LISTING;
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-r");
					aux[2] = strdup("-h");
					while ((ent = readdir(dir)) != NULL) {
						aux[3] = strdup(ent->d_name);
						cmd_list(aux, 4);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		case 8:
			if (nargs > 4) {
				for (i = 4; i<nargs; i++ ) {
					if ((dir = opendir(".")) != NULL) {
						if (!stat(flags[i], &path_stat)) {
							while ((ent = readdir(dir)) != NULL) {
								if (!strcmp(ent->d_name, flags[i])) {
									stat(ent->d_name, &path_stat);
									printf("%s %ld\n", ent->d_name, path_stat.st_size);
									if (S_ISDIR(path_stat.st_mode)) {
										if (strcmp(ent->d_name, "..") && strcmp(ent->d_name,".")) {
											chdir(ent->d_name);
											aux[1] = strdup("-r");
											aux[2] = strdup("-n");
											aux[3] = strdup("-h");
											cmd_list(aux, 4);
											chdir("..");
										}
									}
								}
							}
						} else {
							printf("cannot access %s: %s\n", flags[i], strerror(errno));
						}
						closedir(dir);
					}
					else {
						return ERROR_LISTING;
					}
				}
			}
			else {
				if ((dir = opendir(".")) != NULL) {
					aux[1] = strdup("-r");
					aux[2] = strdup("-n");
					aux[3] = strdup("-h");
					while ((ent = readdir(dir)) != NULL) {
						aux[4] = strdup(ent->d_name);
						cmd_list(aux, 5);
					}
					closedir(dir);
				}
				else {
					return ERROR_LISTING;
				}
			}
			break;
		default:
			break;
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