#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#define DEFAULT_BUFFER_SIZE 256 
#define HELP_MSG "Usage: mycp_sc filein fileout [buffersize]\nDefault buffersize is 256 bytes\n"

typedef enum {false, true} bool;

//Checks if 'str' contains a "correct" long int
bool isNumber(char* str, long* val){
	if (str == NULL) return false; //MUST check !
	if (strlen(str) == 0) return false; //MUST check ! ##missing
	bool result = false;
	int current = 0;
	int length = strlen(str); // > 0 a questo punto
	if (str[0] == '-') current = 1;
	//Questo controlla già che sia (almeno in teoria) un numero
	while (current < length){
		if (str[current] >= '0' && str[current] <= '9'){
			result = true;
			current++;
		} else {
			result = false;
			break;
		}
	}
	if (result) {
		char* e = NULL;
		errno = 0;
		long v = strtol(str, &e, 10);
		if (errno == ERANGE){ result = false; } //Overflow
		if (result) {*val = v;}
	}
	return result;
}

int main(int argc, char* argv[]){
	long size;
	if (argc < 3){
		fprintf(stderr, "Error: too few arguments\n");
		printf(HELP_MSG);
		return -1;
	} else if (argc == 4) {
		size = DEFAULT_BUFFER_SIZE;
	} else if ((!isNumber(argv[2],&size)) || (size <= 0)){
		fprintf(stderr, "Error: third argument must be a positive integer!\n");
		return -1;		
	}
	size_t bufsize = (size_t)size;
	FILE* fd_in = fopen(argv[1], "r"); //Input file to be copied
	if (fd_in == NULL) { 
		fprintf(stderr, "When opening '%s'...\n", argv[1]);
		perror("");
		return -1;
	}
	FILE* fd_out = fopen(argv[2],"w");
	if (fd_out == NULL) { 
		fprintf(stderr, "When opening '%s'...\n", argv[2]);
		perror("");
		return -1;
	}
	char* buffer = malloc(bufsize*sizeof(char));
	if (buffer == NULL){ 
		fprintf(stderr, "When copying file '%s'...\n", argv[1]);
		perror("");
		return -1;
	}
	errno = 0;
	ssize_t readFromFile = 0;
	while ((readFromFile = fread(buffer, bufsize, 1, fd_in))!= -1){
		if (fwrite(buffer, bufsize, 1, fd_out) == -1){
			fprintf(stderr, "Error: something went wrong when writing to file '%s'\n", argv[2]); 
			if (errno != 0) errno = 1;
			break;
		}
		if (readFromFile < bufsize) break;
	}
	free(buffer);
	if (fclose(fd_in) == -1) fprintf(stderr, "Error when closing file '%s'...\n", argv[1]);
	if (fclose(fd_out) == -1) fprintf(stderr, "Error when closing file '%s'...\n", argv[2]);
	return errno;
}
