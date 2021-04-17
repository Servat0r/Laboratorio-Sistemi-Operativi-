//Problema dei filosofi a cena (versione con 5 filosofi)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <PMut.h>
#include <time.h>

//Per meditare/mangiare ogni thread impiega un tempo (in nsec) x * (10e9) + y, con x fra 0 e MAXSEC e y fra 0 e MAXNSEC 
#define MAXSEC 0
#define MAXNSEC 1000000000

//Help string
#define USAGE printf("Usage: %s [num [numiter]], where num is an integer >= 2 and numiter is a positive integer\n", argv[0]); return -1;

//Numero totale di thread
#define N 10

//#Iterazioni per thread
#define NUMITER 15



pthread_mutex_t mtxUp = PTHREAD_MUTEX_INITIALIZER;
int n = N; //default
int numiter = NUMITER;
pthread_cond_t* conds;
bool* busy;

void printForks(void) {
	printf("forks: ");
	for (int i = 0; i < n; i++) printf("%d, ", busy[i]);
	printf("\b\b \n");
}

typedef struct timespec timespec;


//Entrambe le attività "eat" e "meditate"
int activity(void){
	unsigned int seed = time(0);
	timespec* t = malloc(sizeof(timespec));
	t->tv_sec = 0; //(rand_r(&seed) % MAXSEC);
	t->tv_nsec = (rand_r(&seed) % MAXNSEC);
	//TODO Eventualmente gestire con perror
	int res = nanosleep(t, NULL);
	free(t);
	return res;
}



void* philosopher(void* arg){
	size_t id = *((int*)arg);
	for (int j = 0; j < numiter; j++){

		if (activity() != 0) {
			fprintf(stderr, "thread [%lu] while ", id);
			perror("meditating");
		} else {
			printf("thread [%lu] has meditated for %d time\n", id, j + 1);
		}

		PMLock(&mtxUp);
		while (busy[id] || busy[id+1]) pthread_cond_wait(&conds[id], &mtxUp);
		busy[id] = true;
		busy[id+1] = true;
		printForks();
		PMUnlock(&mtxUp);

		if (activity() != 0) {
			fprintf(stderr, "thread [%lu] while ", id);
			perror("eating");
		} else {
			printf("thread [%lu] has eaten for %d time\n", id, j + 1);
		}

		PMLock(&mtxUp);
		busy[id] = false;
		busy[id+1] = false;
		for (int r = 0; r < N; r++) pthread_cond_signal(&conds[r]);
		printForks();
		PMUnlock(&mtxUp);
	}
	printf("Thread [%lu] has finished\n", id);
	return NULL;
}


int main(int argc, char* argv[]){

	if (argc >= 2){
		n = atoi(argv[1]);
		if (n <= 1){
			USAGE;
		}
		if (argc >= 3){
			numiter = atoi(argv[2]);
			if (numiter <= 0){
				USAGE;
			}
		}
	}

	pthread_t tids[n-1];
	int ids[n-1];
	conds = malloc(n * sizeof(pthread_cond_t));
	busy = malloc(n * sizeof(bool));

	for (int i = 0; i < n; i++){
		pthread_cond_init(&conds[i], NULL);
		busy[i] = false;
	}

	for (int i = 0; i < n-1; i++){
		ids[i] = i;
		pthread_create(&tids[i], NULL, &philosopher, &ids[i]);
	}

	for (int j = 0; j < numiter; j++){
		if (activity() != 0){
			printf("thread [%d] while ", n-1);
			perror("meditating");
		} else {
			printf("thread [%d] has meditated for %d time\n", n-1, j + 1);
		}
		
		PMLock(&mtxUp);
		while (busy[0] || busy[n-1]) pthread_cond_wait(&conds[n-1], &mtxUp);
		busy[0] = true;
		busy[n-1] = true;
		printForks();
		PMUnlock(&mtxUp);

		if (activity() != 0) {
			fprintf(stderr, "thread [%d] while ", n-1);
			perror("eating");
		} else {
			printf("thread [%d] has eaten for %d time\n", n-1, j + 1);
		}

		PMLock(&mtxUp);
		busy[0] = false;
		busy[n-1] = false;
		for (int r = 0; r < n; r++) pthread_cond_signal(&conds[r]);
		printForks();	
		PMUnlock(&mtxUp);
	}
	printf("Thread [%d] has finished\n", n-1);
	
	for (int i = 0; i < n-1; i++) pthread_join(tids[i], NULL);
	//for (int i = 0; i < n; i++) pthread_cond_destroy(&conds[i]);
	free(conds);
	free(busy);
	return 0;
}
