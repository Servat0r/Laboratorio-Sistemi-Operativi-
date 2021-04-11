#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define STD_T 15

int n;
unsigned int seconds = STD_T;
pid_t pid;

int main(int argc, char* argv[]){
	if (argc < 2) {
		printf("Usage: <path>/zombieMaker <int(>0)> [<int(>0)>] \n");
		return -1;	
	} else {
		n = atoi(argv[1]);
		if (argc > 2) {
			seconds = atoi(argv[2]);
		}
	}
	int i = 0;
	while (i < n){
		pid = fork();
		if (pid == 0) exit(0);
		i++;
	}
	if (n > 0) sleep(seconds);
	return 0;
}
