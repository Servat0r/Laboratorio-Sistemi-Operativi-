#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if (argc < 2){
		perror("Error: must pass a filepath!\n");
		return -1;
	} else if (argc != 2){
		fprintf(stderr, "Error: too much arguments. Need 1, given %d\n", argc-1);
		return -1;
	}
	FILE* output = fopen(argv[1], "w");
	if (output == NULL) {
		perror("Error: cannot create an output file\n");
		return 1;
	}
	FILE* input = fopen("/etc/passwd", "r");
	if (input == NULL) {
		perror("Error: cannot open file 'etc/passwd'\n");
		return 1;
	}
	char* s = malloc(1024*sizeof(char));
	if (s == NULL){
		perror("Error: not enough memory available to continue execution\n");
		return 2;
	}
	while ((fgets(s,1024,input)) != NULL){
		char* colon_pos = strchr(s, ':');
		if (colon_pos != NULL){ colon_pos[0] = '\0'; } //The string to be copied ends here
		fprintf(output,s);
		fprintf(output,"\n");
	}
	fclose(input);
	fclose(output);
	free(s);
	return 0;
}
