#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PANIC_SYSCALL(expr, res, error) if ((expr) != (res)) { \
	perror(error); exit(1); \
}

#define MAX_INPUT_SIZE 4096 /* 4K (_PC_PIPE_BUF == 4096 here) */

bool isNumber(char* str){
	if (str == NULL) return false;
	if (strlen(str) == 0) return false;
	bool result = false;
	bool dot = false; /* For floating-point numbers */
	int current = 0;
	int length = strlen(str);
	if (str[0] == '-') current = 1;
	while (current < length){
		if (str[current] >= '0' && str[current] <= '9'){
			result = true;
			current++;
		} else if (!dot) { /* First dot (for a fp number) */
			dot = true;
			current++;
		} else {
			result = false;
			break;
		}
	}
	return result;
}

void closeChannel(int channel[2]){
	close(channel[0]);
	close(channel[1]);
}

void clearString(char* string){
	int n = strlen(string);
	strncpy(string, "\0", n);
}

bool checkExit(char* input){
	if (strlen(input) != 5) return false;
	return ((strncmp(input, "exit\n", 5) == 0) || (strncmp(input, "quit\n", 5) == 0));
}

int pipe1[2];
int pipe2[2];
char input[MAX_INPUT_SIZE]; /* For input of data */
char output[MAX_INPUT_SIZE]; /* For output from 'bc' */
pid_t pid;
char prompt[12] = "Operazione:"; /* Prompt */
int status; /* For waiting 'bc' */

void cleanupMain(void){
	close(pipe1[1]);
	close(pipe2[0]);
}

void cleanupBC(void){
	close(pipe2[1]);
	close(pipe1[0]);
}

bool isUseless(char* input){
	bool res = true;
	size_t n = strlen(input);
	for (int i = 0; i < n; i++){
		if (isspace(input[i]) == 0){
			res = false;
			break;
		}
	}
	return res;
}

int main(void){

	
	while (true){
		clearString(input);
		clearString(output);
		printf("%s ", prompt);

		/* Ignore errors (e.g., users does not type anything) */
		if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL){
			perror("reading on stdin");
			continue;
		}

		if (isUseless(input)) continue;

		/* Add '\n' if there is NOT in input */
		if (strlen(input) + 1 == MAX_INPUT_SIZE) {
			if (MAX_INPUT_SIZE > 1) input[MAX_INPUT_SIZE - 2] = '\n';
		}

		/* Check if user wants to exit */
		if (checkExit(input)) break;

		/* If any pipe creation goes wrong... */
		PANIC_SYSCALL(pipe(pipe1), 0, "creazione prima pipe");
		PANIC_SYSCALL(pipe(pipe2), 0, "creazione seconda pipe");
		/* All pipes have been successfully created! */
		
		pid = fork();
		if (pid == -1){
			perror("fork");
			exit(1);	
		} else if (pid == 0) {
			PANIC_SYSCALL(atexit(cleanupBC), 0, "atexit on child");
			PANIC_SYSCALL(dup2(pipe1[0], 0), 0, "redirezione stdin");
			PANIC_SYSCALL(dup2(pipe2[1], 1), 1, "redirezione stdout");
			PANIC_SYSCALL(dup2(pipe2[1], 2), 2, "redirezione stderr");
			close(pipe1[1]);
			close(pipe2[0]);
			execlp("bc", "bc", "-lq", (char*)NULL);
			perror("bc"); /* Writes on pipe2 */
			read(pipe1[0], input, MAX_INPUT_SIZE); /* Reads parent input */
			exit(1); /* This exit now should be correctly caught */
		} else {
			close(pipe1[0]); /* Does NOT read on the first pipe */
			close(pipe2[1]); /* Does NOT write on the second pipe */
			ssize_t res;
			res = write(pipe1[1], input, strlen(input));
			if (res < 0) {
				perror("write");
				write(pipe1[1], "quit\n", MAX_INPUT_SIZE);
				wait(&status);
				cleanupMain();
				exit(1);
			}
			res = read(pipe2[0], output, MAX_INPUT_SIZE);
			/* A "\0" output does not make any sense ... */
			if (strlen(output) > 0){
				/* There is a '\n' at this position */
				output[strlen(output) - 1] = '\0';
				/* Checks if the output is a number */
				if (!isNumber(output)){
					fprintf(stderr, "%s ", output);
					/* Avoid printing garbage from the previous message */
					clearString(output);
					res = read(pipe2[0], output, MAX_INPUT_SIZE);
					fprintf(stderr, "%s\n", output);
				} else {
					printf("Risultato: %s\n", output);
				}
			}
			close(pipe1[1]); /* Equivalent to Ctrl-D for 'bc' */
			wait(&status);
			close(pipe2[0]);	
			#if defined(_DEBUG_)
			if (WIFEXITED(status)){
				printf("Exited with status %d\n", WEXITSTATUS(status));			
			} else if (WIFSIGNALED(status)){
				printf("Terminated by signal %d\n", WTERMSIG(status));
			}
			#endif
		}
	}
	printf("Exiting...\n");
	return 0;
}
