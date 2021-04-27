#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXSIZE 4096 /* 4 KB (== _PC_PIPE_BUF) */

#define PANIC_SYSCALL(expr, res, error) if ((expr) != (res)) { \
	perror(error); exit(1); \
}

/* Help string */
#define USAGE "Usage: %s num, where num is a positive integer.\n"

/* Close an ENTIRE pipe */
void closeChannel(int channel[2]){
	close(channel[0]);
	close(channel[1]);
}

int pipe1[2]; /* child1 has pipe1[1] as stdout and pipe2[0] as stdin */
int pipe2[2]; /* child2 has pipe2[1] as stdout and pipe1[0] as stdin */
pid_t pid1; /* child1 */
pid_t pid2; /* child2 */
int status; /* waiting childs */

int main(int argc, char* argv[]){
	
	/* Check (x > 0) */
	int x = 0;
	if (argc > 1){
		x = atoi(argv[1]);
	}
	if (x <= 0){
		fprintf(stderr, USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Pipe creation */
	PANIC_SYSCALL(pipe(pipe1), 0, "pipe1");
	PANIC_SYSCALL(pipe(pipe2), 0, "pipe2");

	/* First child (argv[1] passed) creation */
	switch(pid1 = fork()){
		case -1 : {
			perror("fork1");
			exit(errno);
			break;
		}
		case 0 : { /* Child1 */
			/* Redirecting stdin and stdout */
			PANIC_SYSCALL(dup2(pipe2[0],0), 0, "child1 - redirecting stdin");
			PANIC_SYSCALL(dup2(pipe1[1],1), 1, "child1 - redirecting stdout");

			/* Closing pipes */
			closeChannel(pipe1); closeChannel(pipe2);
			
			execlp("bin/dec", "bin/dec", argv[1], (char*)NULL);
			/* FIXME Is this exit CORRECTLY caught?? */			
			perror("child1 - dec");
			exit(errno);			
		}
		default : { /* Parent */
			switch(pid2 = fork()){
				case -1 : {
					perror("fork2");
					exit(errno);
					break;				
				}
				case 0 : { /* Child2 */
					/* Redirecting stdin and stdout */
					PANIC_SYSCALL(dup2(pipe1[0],0), 0, "child2 - redirecting stdin");
					PANIC_SYSCALL(dup2(pipe2[1],1), 1, "child2 - redirecting stdout");
					
					/* Closing pipes */
					closeChannel(pipe1); closeChannel(pipe2);
					
					execlp("bin/dec", "bin/dec", (char*)NULL);
					/* FIXME Is this exit CORRECTLY caught?? */
					perror("child2 - dec");
					exit(errno);
				}
				default : { /* Parent */
					/* This way we are SURE no SIGPIPEs */
					closeChannel(pipe1);
					closeChannel(pipe2);
					pid_t exitpid;
					for (int j = 0; j < 2; j++){
						exitpid = wait(&status);
						if (WIFEXITED(status)){
							printf("#%d exited with status - %d\n", exitpid, WEXITSTATUS(status));
						} else if (WIFSIGNALED(status)){
							printf("#%d terminated by signal - %d\n", exitpid, WEXITSTATUS(status));
						}
					}
				} 
			}
		}
	}
	return 0;
}
