#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <PMut.h>
#include <chopsticks.h>

chopsticks_t* chopsticks_init(size_t n){
	chopsticks_t* f = malloc(sizeof(chopsticks_t));
	if (f == NULL) return NULL;
	
	f->busy = malloc(n * sizeof(bool));
	if (f->busy == NULL){
		free(f);
		return NULL;
	}
	f->size = n;
	pthread_mutex_init(&(f->mtx), NULL);
	pthread_cond_init(&(f->cond), NULL);
	pthread_barrier_init(&(f->barrier), NULL, n);
	for (int i = 0; i < n; i++) f->busy[i] = false;
	return f;
}

bool chopsticks_ask(chopsticks_t* f, int id){
	PMLock(&(f->mtx));
	while (f->busy[id] || f->busy[id+1]) pthread_cond_wait(&(f->cond), &(f->mtx));
	f->busy[id] = true;
	f->busy[id+1] = true;
	PMUnlock(&(f->mtx));
	return true;
}

bool chopsticks_release(chopsticks_t* f, int id){
	PMLock(&(f->mtx));
	f->busy[id] = false;
	f->busy[id+1] = false;
	pthread_cond_broadcast(&(f->cond));
	PMUnlock(&(f->mtx));
	return true;
}

void chopsticks_destroy(chopsticks_t* f){
	PMLock(&(f->mtx));
	//while (pthread_barrier_destroy(&(f->barrier)) != 0) pthread_cond_wait(&(f->cond), &(f->mtx));
	pthread_barrier_destroy(&(f->barrier));	
	free(f->busy);
	pthread_cond_destroy(&(f->cond));
	pthread_mutex_destroy(&(f->mtx));
	free(f);
}

void chopsticks_abandon(chopsticks_t* f){
	pthread_barrier_wait(&(f->barrier));
}
