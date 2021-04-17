/*
Una versione più estesa dell'esercizio in cui il buffer ha capacità n
(passata eventualmente come parametro)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <PMut.h>
#include <errno.h>

#if !defined(N)
#define N 3
#endif

#if !defined(LIMIT)
#define LIMIT 10
#endif

int first = 0; //Prima posizione libera
int iter = 1; //Massimo numero globale di iterazioni

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t putcond = PTHREAD_COND_INITIALIZER;
pthread_cond_t getcond = PTHREAD_COND_INITIALIZER;


void* get(void* arg){
	int limit = ((int*)arg)[1];
	int* buffer = ((int*)arg) + 2;
	while (true){
		PMLock(&mtx);
		int myiter = iter++;
		printf("get - myiter = %d\n", myiter);
		if (myiter > limit){
			pthread_cond_signal(&putcond);
			PMUnlock(&mtx);
			break;
		}
		while (first == 0 && iter <= limit + 1) pthread_cond_wait(&getcond, &mtx);
		if (iter > limit + 1){
			pthread_cond_signal(&putcond);
			PMUnlock(&mtx);
			printf("get - Aborted cause having reached maximum number of iterations\n");
			break;
		}
		int ret = buffer[--first];
		buffer[first] = 0; //No reads in memory
		pthread_cond_signal(&putcond);
		PMUnlock(&mtx);
		printf("\nExtracted : %d\n", ret);
	}
	printf("Thread get exiting...\n");
	return NULL;
}

void* put(void* arg){
	int n = ((int*)arg)[0];
	int limit = ((int*)arg)[1];
	int* buffer = ((int*)arg) + 2;
	static int next = 0;
	while (true){
		PMLock(&mtx);
		int myiter = iter++;
		int elem = ++next; //Produce il prossimo elemento
		printf("put - myiter = %d, (elem = %d)\n", myiter, elem);
		if (myiter > limit){
			pthread_cond_signal(&getcond);
			PMUnlock(&mtx);
			break;
		}
		while (first == n && iter <= limit + 1){
			pthread_cond_wait(&putcond, &mtx);
		}
		if (iter > limit + 1){
			pthread_cond_signal(&getcond);
			PMUnlock(&mtx);
			printf("put - Aborted cause having reached maximum number of iterations\n");
			break;
		}
		buffer[first++] = elem; //Scrive sul buffer
		pthread_cond_signal(&getcond);
		PMUnlock(&mtx);
	}
	printf("Massimo valore raggiunto di next: %d\n", next);
	printf("Thread put exiting...\n");
	return NULL;
}

int main(int argc, char* argv[]){
	int n = N;
	int limit = LIMIT;
	pthread_t tid1;
	pthread_t tid2;
	if (argc >= 2){
		n = atoi(argv[1]);
		if (n <= 0){
			printf("Usage: %s [(num > 0) [(limit > 0)]]\n", argv[0]);
			return 1;
		}
		if (argc >= 3){
			limit = atoi(argv[2]);
			if (limit <= 0){
				printf("Usage: %s [(num > 0) [(limit > 0)]]\n", argv[0]);
				return 1;
			}
		}
	}
	int ext_buffer[n+2]; //Primo termine contiene n, gli altri gli elementi
	ext_buffer[0] = n;
	ext_buffer[1] = limit;
	for (int i = 2; i < n+2; i++) ext_buffer[i] = 0; //Inizializzazione a 0	
	pthread_create(&tid2, NULL, &put, ext_buffer);
	pthread_create(&tid1, NULL, &get, ext_buffer);	
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Done\n");	
	return 0;
}
