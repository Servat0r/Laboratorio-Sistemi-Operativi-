//Inia, inia, alalà!

/*
Options:
mywc [-l -w] file1 [file2 ... ] -> gives lines and words count of each file

mywc -l file1 [file2 ... ] -> gives lines count of each file

mywc -w file1 [file2 ... ] -> gives words count of each file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define RES0 "For file %s there are: "
#define RESL "%u lines "
#define RESW "%u words "

typedef enum {false, true} bool;

void count(FILE* file, unsigned int* lines, unsigned int* words, bool foundL, bool foundW){
	
	char* buffer = malloc(1024*sizeof(char));
	if (foundL) *lines = 0;
	if (foundW) *words = 0;
	//printf("Ciao a tutti\n");
	bool scanning = false;
	while (fgets(buffer, 1024, file) != NULL){
		size_t len = strlen(buffer); // > 1 by hypothesis
		if (buffer[len - 1] == '\n') *lines += 1;
		for (int i = 0; i < len; i++){
			if (isspace(buffer[i]) == 0){ //NON è uno spazio
				if (!scanning) *words += 1; //Inizio di un'altra parola
				scanning = true;
			} else {
				scanning = false;
			}
		}
	}
	rewind(file);

	return;
}


int main(int argc, char** argv){

	if (argc < 2){
		fprintf(stderr,"Error: too few arguments\n");
		return -1;
	}

	int opt;
	int returnCode = 0;
	bool foundL = false;
	bool foundW = false;
	optind = 1; 
	//Remember to consider also extra agument (more files provided)
	while ((opt = getopt(argc, argv, ":lw")) != -1){
		switch(opt){
			case '?':
				fprintf(stderr, "Error: unrecognized option '-%c'\n", optopt);
				return -1;
			case 'l':
				if (!foundL) foundL = true;
				else fprintf(stderr, "Warning: duplicated option '-l'\n");
				break;
			case 'w':
				if (!foundW) foundW = true;
				else fprintf(stderr, "Warning: duplicated option '-w'\n"); 
				break;
			case ':':				
				break;
		}
	}
	if (!(foundL || foundW)){
		foundL = true;
		foundW = true;
		optind = 1;
	}
	FILE* fptr;
	unsigned int lines = 0;
	unsigned int words = 0;
	for(;optind < argc; optind++){
		fptr = fopen(argv[optind],"r");
		if (fptr == NULL){
			fprintf(stderr, "Error: unexisting file '%s'\n", argv[optind]);	
			continue;
		}
		count(fptr,&lines,&words,foundL,foundW);
		printf(RES0, argv[optind]);
		if (foundL) printf(RESL, lines);
		if (foundW) printf(RESW, words);
		printf("\n");
		fclose(fptr);		
	}
	return 0;
}
