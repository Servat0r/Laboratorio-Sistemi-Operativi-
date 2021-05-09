#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include <myutils.h>
#include <limits.h>
#define MAXSIZE 4096

/*
#verbose - toggles on/off verbose mode
#help - prints a help message
#ans - last input
*/

//Options flags
bool verbose = false;

char buffer[MAXSIZE];
char prompt[MAXSIZE];
char* myargv[MAXSIZE];
size_t myargc;
int status;
pid_t pid;

bool master = true;

int main(int argc, char** argv){
	if ((argc == 2) && (strncmp(argv[1], "--verbose", 9) == 0)){
		verbose = true;
	} else if (argc != 1){
		printf("Usage: <path>/dummyshell [--verbose]");
		return -1;
	}

	while(true){
		pid = 0;
		errno = 0;
		if (getcwd(prompt, MAXSIZE) == NULL){
			perror("cwd");
			break;
		}
		size_t n = strlen(prompt) + 3;
		if (n > MAXSIZE){
			errno = ERANGE;
			perror("showing prompt");
			break;		
		}
		strncat(prompt, "$ ", n);
		if (mygetline(buffer, prompt, MAXSIZE) == NULL){
			perror("mygetline");
			break;
		}
		myargc = getcmd(buffer, myargv, MAXSIZE);
		if (myargc == -1){
			fprintf(stderr, "Invalid command input!\n");
			continue;
		} else if (myargc == 0){
			continue;
		}
		//CheckExit & CheckChdir
		if (checkExit(myargv[0])) break;
		if (checkChdir(myargv[0])){
			chdir(myargv[1]);
			continue;
		}
		pid = fork();
		if (pid == 0){
			execvp(myargv[0], myargv);
			master = false;
			fprintf(stderr, "When executing command '%s': ", myargv[0]);
			perror("");
			exit(1);
		} else {
			if (waitpid(pid, &status, 0) == -1){
				fprintf(stderr,"When waiting process %d: ", pid);
				perror("");
				exit(EXIT_FAILURE);
			}
			if (verbose){
				printf("Process [%d] terminated. ", pid);
				if (WIFEXITED(status)){
					printf("Status: %d\n", WEXITSTATUS(status));
				} else {
					printf("No exit status available\n");
				}
			}
			#ifdef DBGM
			printf("myargc = %lu\n", myargc);
			for (int i = 0; i < myargc; i++)
				printf("myargv[%d] = '%s' ", i, myargv[i]);
			printf("\n");
			#endif
		}
	}
	return 0;
}
