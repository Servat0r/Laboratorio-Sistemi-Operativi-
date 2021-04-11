#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define LINE "-"

int family(int n){
	pid_t pid = getpid();
	for (int i = 0; i < n; i++) printf("%s", LINE);
	if (n == 0){
		printf("%d: sono l'ultimo discendente\n", pid);
		printf("%d: terminato con successo\n", pid);
		return 0;
	} else {
		printf(" %d: creo un processo figlio\n", pid);
		fflush(stdout);
		pid_t cpid = fork();
		if (cpid == 0){ //Child
			return family(n-1);
		} else {
			int status;
			if (waitpid(cpid, &status, 0) == -1){
				perror("waiting");
				return -1;
			}
			for (int i = 0; i < n; i++) printf("%s", LINE);
			if (WIFEXITED(status)){				
				if (WEXITSTATUS(status) == 0) printf(" %d: terminato con successo\n", pid);
				else printf(" %d: terminato con fallimento\n", pid);
			}
			return WEXITSTATUS(status);
		}
	}
}

//Manca il controllo di isNumber
int main(int argc, char* argv[]){
	if (argc < 2) return -1;
	int n = atoi(argv[1]);
	return family(n);
}
