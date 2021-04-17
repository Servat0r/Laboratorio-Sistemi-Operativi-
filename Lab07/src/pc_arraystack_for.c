/*
Una versione più estesa dell'esercizio in cui il buffer ha capacità n
(passata eventualmente come parametro)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#define N 2
#define LIMIT 3
#define UPDATE_PUT printf("first = %d\n", first);

int n = N; //Capacità totale del buffer
int first = 0; //Prima posizione libera
int buffer[N]; //Buffer

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t getcond = PTHREAD_COND_INITIALIZER;


void PMLock(pthread_mutex_t *mtx) {
	int err;
	if ( ( err=pthread_mutex_lock(mtx)) != 0 ) {
		errno=err;
		perror("lock");
		pthread_exit((void*)&errno); /* maybe */
	}
	else printf("locked ");
}

void PMUnlock(pthread_mutex_t *mtx) {
	int err;
	if ( (err = pthread_mutex_unlock(mtx)) != 0) {
		errno = err;
		perror("lock");
		pthread_exit((void*)&errno);
	}
	else printf("unlocked ");
}


void* get(void* arg){
	for (int i = 0; i < LIMIT; i++){
		PMLock(&mtx);
		printf(" get (i = %d)\n", i);
		while (first == 0){
			pthread_cond_wait(&getcond, &mtx);
		}
		printf("getBefore - first = %d\n", first);
		int ret = buffer[--first];
		buffer[first] = 0; //No reads in memory
		pthread_cond_signal(&putcond);
		printf("getAfter - first = %d\n", first);
		PMUnlock(&mtx);
		printf("\n***Extracted : %d***\n", ret);
	}
	return NULL;
}

void* put(void* arg){
	for (int i = 0; i < LIMIT; i++){
		PMLock(&mtx);
		printf(" put (i = %d)\n", i);
		int elem = i; //Prossimo elemento
		
		while (first == n){
			pthread_cond_wait(&putcond, &mtx);
		}
		printf("putBefore - first = %d\n", first);
		buffer[first++] = elem; //Scrive sul buffer
		pthread_cond_signal(&getcond);
		printf("putAfter - first = %d\n", first);
		PMUnlock(&mtx);
	}
	return NULL;
}

int main(int argc, char* argv[]){
	pthread_t tid1;
	pthread_t tid2;
/*	if (argc >= 2){
		n = atoi(argv[0]);
		if (n <= 0){
			printf("Usage: %d [num > 0]\n", argv[0]);
			return 1;
		}
	}
*/
	pthread_create(&tid1, NULL, &get, NULL);
	pthread_create(&tid2, NULL, &put, NULL);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Done\n");	
	return 0;
}
