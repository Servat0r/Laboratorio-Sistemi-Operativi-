//Inia, inia, alalà!

/*
Options:
mywc [-l -w -c] file1 [file2 ... ] -> gives lines, words and chars count of each file

mywc -l file1 [file2 ... ] -> gives lines count of each file

mywc -w file1 [file2 ... ] -> gives words count of each file

mywc -c file1 [file2 ... ] -> gives chars count of each file
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

#define RES0 "For file %s there are: "
#define RESL "%u lines "
#define RESW "%u words "
#define RESC "%u chars "

typedef enum {false, true} bool;

void count(FILE* file, unsigned int* lines, unsigned int* words, unsigned int* chars, bool foundL, bool foundW, bool foundC){
	
	char* buffer = malloc(1024*sizeof(char));
	if (foundL) *lines = 0;
	if (foundW) *words = 0;
	if (foundC) *chars = 0;
	bool scanning = false;
	while (fgets(buffer, 1024, file) != NULL){
		size_t len = strlen(buffer); // > 1 by hypothesis
		*chars = *chars + len; //Conta TUTTI i caratteri!
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
	bool foundC = false;
	optind = 1; 
	//Remember to consider also extra agument (more files provided)
	while ((opt = getopt(argc, argv, ":lwc")) != -1){
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
			case 'c':
				if (!foundC) foundC = true;
				else fprintf(stderr, "Warning: duplicated option '-c'\n"); 
				break;
			case ':':				
				break;
		}
	}
	if (!(foundL || foundW || foundC)){
		foundL = true;
		foundW = true;
		foundC = true;
		optind = 1;
	}
	FILE* fptr;
	unsigned int lines = 0;
	unsigned int words = 0;
	unsigned int chars = 0;
	for(;optind < argc; optind++){
		fptr = fopen(argv[optind],"r");
		if (fptr == NULL){
			fprintf(stderr, "Error: unexisting file '%s'\n", argv[optind]);	
			continue;
		}
		count(fptr,&lines,&words,&chars,foundL,foundW,foundC);
		printf(RES0, argv[optind]);
		if (foundL) printf(RESL, lines);
		if (foundW) printf(RESW, words);
		if (foundC) printf(RESC, chars);
		printf("\n");
		fclose(fptr);		
	}
	return 0;
}
