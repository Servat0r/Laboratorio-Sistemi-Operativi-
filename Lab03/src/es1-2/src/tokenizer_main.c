#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

int main(int argc, char* argv[]){
	if (argc == 1) {perror("Too few arguments!\n"); return -1; }
	for (int i = 1; i < argc; i++){
		int n = strlen(argv[i]);
		char* copy = strndup(argv[i],n);
		if (copy == NULL) {perror("Out of memory\n"); return -1; }
		printf("Tokenizing with tokenizer...\n");
		tokenizer(argv[i]);
		strncpy(argv[i], copy, n);
		printf("Tokenizing with tokenizer_r...\n");
		tokenizer_r(argv[i]);
		strncpy(argv[i],copy,n); //Non necessaria
		free(copy);
	}
	return 0;
}
