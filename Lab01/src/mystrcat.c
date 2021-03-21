#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {false, true} bool;

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

//Requires: gli argomenti variabili terminano SEMPRE con un 'NULL'
char* mystrcat(char* buffer, size_t buffer_size, char* prima, ...){
	buffer = strncat(buffer, prima, buffer_size);
	if (strlen(prima) >= buffer_size){
		buffer[buffer_size - 1] = 0;
		return buffer;
	}
	size_t current = strlen(prima);
	va_list ap;
	char* s;
	va_start(ap, prima);
	while (true) {
		s = va_arg(ap, char*);
		if (s == NULL) break; // !!!
		buffer = strncat(buffer, s, buffer_size - current);
		if (current + strlen(s) >= buffer_size){
			buffer[buffer_size - 1] = 0;
			return buffer;
		} else {
			current += strlen(s);
		}
	}
	return buffer;
}

int main(int argc, char* argv[]){
	if (argc < 7) {
		printf("Troppo pochi argomenti\n");
		return -1;
	}
	char* buffer = NULL;
	RIALLOCA(&buffer, REALLOC_INC); //macro che effettua l'allocazione del 'buffer'	
	buffer[0] = '\0';
	buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
	printf("%s\n", buffer);
	free(buffer);
	return 0;
}
