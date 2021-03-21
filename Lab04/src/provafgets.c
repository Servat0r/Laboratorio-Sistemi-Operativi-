//Prove varie con fgets

/*
1) Calcoli strlen(buffer);

2) Se buffer[strlen(buffer) - 1] != '\n', allora la linea è incompleta, e crei un
nuovo buffer finché la condizione non si verifica;

3) Sommi le strlen dei buffer per avere la dimensione ESATTA da allocare, chiami
una realloc e fai le strcat (?) necessarie per avere il buffer giusto (se serve,
altrimenti operi direttamente in-place con i buffer creati)

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void){
	char* p = malloc(23 * sizeof(char));
	char* q = malloc(20 * sizeof(char));
	char* r = malloc(19 * sizeof(char));
	FILE* fptr = fopen("./prova.c","r");
	FILE* aux = fptr;
	fgets(p, 21, fptr);
	fgets(q, 20, fptr);
	fgets(r, 19, fptr);
	//printf("P%sP",p);
	//printf("Q%sQ",q);
	//printf("R%sR",r);
	printf("%lu %lu %lu\n",strlen(p),strlen(q),strlen(r));
	return 0;
}
