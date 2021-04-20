//Problema dei filosofi a cena (versione con 5 filosofi)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <PMut.h>
#include <time.h>
#include <chopsticks.h>

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
	chopsticks_t* f = ((void**)arg)[0];
	int* id = ((void**)arg)[1];
	for (int j = 0; j < numiter; j++){
		if (activity() != 0) {
			fprintf(stderr, "thread [%d] while ", *id);
			perror("meditating");
		} else {
			printf("thread [%d] has meditated for %d time\n", *id, j + 1);
		}
		chopsticks_ask(f, *id);
		if (activity() != 0) {
			fprintf(stderr, "thread [%d] while ", *id);
			perror("eating");
		} else {
			printf("thread [%d] has eaten for %d time\n", *id, j + 1);
		}
		chopsticks_release(f, *id);
	}
	printf("Thread [%d] has finished\n", *id);
	//free(arg);
	chopsticks_abandon(f);
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

	chopsticks_t* f = chopsticks_init(n);
	
	for (int i = 0; i < n-1; i++){
		void* args[2]; // = calloc(2, sizeof(void*));
		ids[i] = i;
		args[1] = &ids[i];
		args[0] = f;
		pthread_create(&tids[i], NULL, &philosopher, args);
	}

	for (int j = 0; j < numiter; j++){
		if (activity() != 0){
			printf("thread [%d] while ", n-1);
			perror("meditating");
		} else {
			printf("thread [%d] has meditated for %d time\n", n-1, j + 1);
		}
		chopsticks_ask(f, n-1);
		if (activity() != 0) {
			fprintf(stderr, "thread [%d] while ", n-1);
			perror("eating");
		} else {
			printf("thread [%d] has eaten for %d time\n", n-1, j + 1);
		}
		chopsticks_release(f, n-1);
	}
	printf("Thread [%d] has finished\n", n-1);
	pthread_barrier_wait(&(f->barrier));

	chopsticks_destroy(f);
	for (int i = 0; i < n-1; i++) pthread_join(tids[i], NULL);
	//for (int i = 0; i < n; i++) pthread_cond_destroy(&conds[i]);
	return 0;
}
