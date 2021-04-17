/*
Una versione più estesa dell'esercizio in cui il buffer ha capacità n
(passata eventualmente come parametro)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <myboundedqueue.h>

#if !defined(LIMIT)
#define LIMIT 8
#endif

#if !defined(MAXQSIZE)
#define MAXQSIZE 5
#endif

#define UPDATE printf("first = %d, i = %d\n", first, i);

//int n = N; //Capacità totale del buffer
int first = 0; //Prima posizione libera
//int buffer[N]; //Buffer
int iter = 1; //Massimo numero globale di iterazioni

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
	BoundedQueue* bq = (BoundedQueue*)arg;
	int* ret;
	while (true){
		PMLock(&mtx);
		int myiter = iter++;
		printf("get - myiter = %d\n", myiter);
		if (myiter > LIMIT){
			pthread_cond_signal(&putcond);
			PMUnlock(&mtx);
			break;
		}
		while (size(*(bq->q)) == 0 && iter <= LIMIT + 1){
			pthread_cond_wait(&getcond, &mtx);
			//FIXME printf("get waiting");
		}
		if (iter > LIMIT + 1){
			pthread_cond_signal(&putcond);
			PMUnlock(&mtx);
			printf("get - Aborted cause having reached maximum number of iterations\n");
			break;
		}
		printf("getBefore - size = %lu\n", size(*(bq->q)));
		ret = (int*)dequeue(bq->q);
		printf("***Extracted : %d***\n", *ret);
		//FIXME printf("get freed (ret = %d)\n", ret);
		pthread_cond_signal(&putcond);
		//FIXME printf("get unlocking (ret = %d)\n", ret);
		printf("getAfter - size = %lu\n", size(*(bq->q)));
		PMUnlock(&mtx);
		
	}
	printf("Thread get exiting...\n");
	return NULL;
}

void* put(void* arg){
	BoundedQueue* bq = (BoundedQueue*)arg;
	static int next = 0;
	while (true){
		PMLock(&mtx);
		int myiter = iter++;
		int* elem = malloc(sizeof(int)); 
		*elem = ++next; //Produce il prossimo elemento
		printf("put - myiter = %d, (elem = %d)\n", myiter, *elem);
		if (myiter > LIMIT){
			pthread_cond_signal(&getcond);
			PMUnlock(&mtx);
			break;
		}
		while (size(*(bq->q)) == bq->n && iter <= LIMIT + 1){
			pthread_cond_wait(&putcond, &mtx);
			//FIXME printf("put waiting (elem = %d)\n", elem);
		}
		if (iter > LIMIT + 1){
			pthread_cond_signal(&getcond);
			PMUnlock(&mtx);
			printf("put - Aborted cause having reached maximum number of iterations\n");
			break;
		}
		printf("putBefore - size = %lu\n", size(*(bq->q)));
		//FIXME printf("put freed (elem = %d)\n", elem);
		enqueue(bq->q, elem);
		pthread_cond_signal(&getcond);
		//FIXME printf("put unlocking (elem = %d)\n", elem);
		printf("putAfter - size = %lu\n", size(*(bq->q)));
		PMUnlock(&mtx);
	}
	printf("Massimo valore raggiunto di next: %d\n", next);
	printf("Thread put exiting...\n");
	return NULL;
}

int main(int argc, char* argv[]){
	int maxqsize = MAXQSIZE;
	pthread_t tid1;
	pthread_t tid2;
	if (argc >= 2){
		maxqsize = atoi(argv[1]);
		if (maxqsize <= 0){
			printf("Usage: %s [maxqsize (>0)]\n", argv[0]);
			return 1;
		}
	}
	BoundedQueue* bq = initBQueue(maxqsize);
	pthread_create(&tid2, NULL, &put, bq);
	pthread_create(&tid1, NULL, &get, bq);	
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Done\n");	
	return 0;
}
