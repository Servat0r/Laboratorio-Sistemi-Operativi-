#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve effettuare fork di processi con il vincolo che 
 * ogni processo esegua 'doFib' al più una volta.  
 * Se l'argomento doPrint e' 1 allora la funzione stampa il numero calcolato 
 * prima di passarlo al processo padre. 
 */
static void doFib(int n, int doPrint){
	if (n <= 2) {
		if (doPrint == 0) exit(1);
		printf("result = %d\n", 1);
		return;
	}
	int prev0;
	int prev1;
	pid_t pid1 = fork();
	if (pid1 == 0){
		doFib(n-1, 0);
	} else {
		pid_t pid2 = fork();
		if (pid2 == 0){
			doFib(n-2, 0);
		} else {
			if (waitpid(pid2, &prev1, 0) == -1){
				perror("fork2");
				exit(-1);
			} else if (waitpid(pid1, &prev0, 0) == -1){
				perror("fork1");
				exit(-1);
			}
			if (WIFEXITED(prev0) && WIFEXITED(prev1)){
				prev0 = WEXITSTATUS(prev0);
				prev1 = WEXITSTATUS(prev1);
			}
			if ((prev0 != -1) && (prev1 != -1)){
				if (doPrint == 0) exit(prev0 + prev1);
				printf("result = %d\n", prev0+prev1);
				return; //Suppose doPrint == 1 <=> initial call
			} else {
				exit(-1);
			}
		}
	}
}

int main(int argc, char *argv[]) {
    // questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.  
    const int NMAX=13;
    int arg;
    
    if(argc != 2){
	fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	return EXIT_FAILURE;
    }
    arg = atoi(argv[1]);
    if(arg <= 0 || arg > NMAX){
	fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
	return EXIT_FAILURE;
    }   
    doFib(arg, 1);
    return 0;
}
