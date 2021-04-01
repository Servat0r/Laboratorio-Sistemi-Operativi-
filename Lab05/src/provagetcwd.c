#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define UNIT_SIZE 1024 //1023 caratteri

char* cwd(){
	int i = 1;
	char* buffer = malloc(UNIT_SIZE);
	while (1){
		char* res = getcwd(buffer, i * UNIT_SIZE);
		if (res == NULL){
			switch(errno){
				case ERANGE:
					buffer = realloc(buffer, (++i) * UNIT_SIZE);
					break;
				default:
					return NULL;
			}	
		} else {
			return buffer;
		}
	}
}


int main(int argc, char** argv){
	if (argc < 2){
		printf("Usage: provagetcwd <dir_name>\n");
		return -1;
	}
	char* buffer = cwd();
	if (buffer == NULL){
		perror("When getting cwd");
		return -1;
	} else {
		printf("Current working directory is: %s\n", buffer);
		free(buffer);
		int r = chdir(argv[1]);
		if (r != 0){
			perror("When changing directory...\n");
			return -1;
		}
		buffer = cwd();
		printf("Current working directory is: %s\n", buffer);
		free(buffer);
		return 0;
	}
}
