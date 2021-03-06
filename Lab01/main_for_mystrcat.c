#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mystrcat.h"

const int REALLOC_INC = 16;

void RIALLOCA(char** buf, size_t newsize){
	char* re;
	if (*buf == NULL){
		re = (char*)malloc(newsize);
	} else {
		re = malloc(sizeof(*buf));
		sscanf(*buf, "%s", re);
		re = realloc(re, newsize);
		free(*buf);
	}
	if (re == NULL){
		perror("Impossibile riallocare la memoria\n");
		exit(1);
	} else {
		*buf = re;
	}
}

/*
TEST DA FARE:
[1] argc < 7 OK
[2] (argc == 7) AND |argv[1]| >= 16
[3] (argc = 7) AND |argv[
[4]
[5]
[6]
*/
int main(int argc, char* argv[]){
	if (argc < 7) {
		printf("Troppo pochi argomenti\n");
		return -1;
	}
	//char* buffer = "Sal";
	//RIALLOCA(&buffer, REALLOC_INC);
	//printf("%s\n", buffer);
	char* buffer = NULL;
	RIALLOCA(&buffer, REALLOC_INC); //macro che effettua l'allocazione del 'buffer'	
	buffer[0] = '\0';
	buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
	printf("%s\n", buffer);
	free(buffer);
	return 0;
}
/*
int main(void){
	char* p = malloc(10*sizeof(char));
	sscanf("Salvatore", "%s", p);
	RIALLOCA(&p, 20*sizeof(char)); //Riallochiamo a uno spazio di 20 caratteri
	p = strcat(p, " Correnti");
	printf("%s\n", p);
	return 0;
}
*/
