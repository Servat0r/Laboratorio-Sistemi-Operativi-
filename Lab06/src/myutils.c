#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <myutils.h>

#define N 1024 //1KB
#define MAX_PATH_SIZE 1024 //1KB

char* mygetline(char* buffer, char* prompt, size_t n){
	printf("%s", prompt);
	size_t j = 0;
	fflush(stdout);
	if (fgets(buffer, n, stdin) == NULL){
		return NULL;
	} else {
		j = strlen(buffer);
		if (buffer[j-1] == '\n') buffer[j-1] = '\0';
		else return NULL;
	}
	return buffer;
}

bool checkExit(char* filename){
	if (strlen(filename) != 4) return false;
	return (strncmp(filename, "exit", 4*sizeof(char)) == 0 ? true : false);
}

bool checkChdir(char* filename){
	if (strlen(filename) != 2) return false;
	return (strncmp(filename, "cd", 2*sizeof(char)) == 0 ? true : false);
}

bool checkBkgr(int argc, char* argv[]){
	if (strlen(argv[argc-1]) != 1) return false;
	return ( strncmp(argv[argc-1],"&",1) == 0 ? true : false );
}

static size_t getArgc(char* buffer){
	size_t res = 0;
	bool space = true;
	for (int i = 0; i < strlen(buffer); i++){
		if (isspace(buffer[i]) != 0){
			buffer[i] = ' '; //TODO Can cause bugs!
			space = true;
		} else if (space){
			res++; space = false;
		}	
	}
	return res;
}

//Gets a filename from a path of the form "*/<name>"
char* getfilename(char* pathname){
	size_t n = strlen(pathname);
	int j = -1;
	for (int i = 0; i < n; i++){
		if (pathname[i] == '/') j = i;
	}
	char* filename = malloc((n - (++j) + 1)*sizeof(char));
	if (filename == NULL) return NULL;
	strncpy(filename, pathname + j, n-j);
	return filename;
}

int getcmd(char* buffer, char* argv[], size_t maxsize){
	int argc = getArgc(buffer);
	if (argc >= maxsize) return -1;
	char* tmpstr;
	char* token = strtok_r(buffer, " ", &tmpstr);
	int i = 0;
	while (token) {
		argv[i] = token; //malloc(1024 * sizeof(char));
		//strncpy(argv[i], token, strlen(token) + 2);
		token = strtok_r(NULL, " ", &tmpstr);
		i++;
	}
	argv[argc] = (char*)NULL;
	return argc;
}
