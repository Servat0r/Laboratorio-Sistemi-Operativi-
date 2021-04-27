#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <tsqueue.h>
#include <pthread.h>
#include <PMut.h>

tsqueue_t* tsqueue_init(void){
	tsqueue_t* q = malloc(sizeof(tsqueue_t));
	if (q == NULL) return NULL;
	if (pthread_mutex_init(&(q->lock), NULL) != 0) {
		free(q); return NULL;
	}
	if (pthread_cond_init(&(q->putVar), NULL) != 0) { 
		free(q);
		pthread_mutex_destroy(&(q->lock));
		return NULL;		
	}
	if (pthread_cond_init(&(q->getVar), NULL) != 0) { 
		free(q);
		pthread_mutex_destroy(&(q->lock));
		pthread_cond_destroy(&(q->putVar));
		return NULL;		
	}		
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	q->waitPut = 0;
	q->waitGet = 0;
	q->activePut = false;
	q->activeGet = false;
	q->ended = false;
	return q;
}

static size_t tsqueue_size(tsqueue_t* q){
	size_t res = q->size;
	return res;
}


static bool tsqueue_isEmpty(tsqueue_t* q){ 
	return (q->size == 0);
}

static bool putShouldWait(tsqueue_t* q){
	bool res = ((!q->ended) && (q->activePut || q->activeGet));
	return res;
}

static bool getShouldWait(tsqueue_t* q){
	bool res = ((!q->ended) && ((tsqueue_size(q) == 0) || q->activePut || q->activeGet));
	return res;
}


bool tsqueue_put(tsqueue_t* q, void* elem){
	if (q == NULL) {
		return false;
	} else {
		PMLock(&(q->lock));
		q->waitPut++;
		while (putShouldWait(q)){
			pthread_cond_wait(&(q->putVar), &(q->lock));
		}
		if (q->ended){
			PMUnlock(&(q->lock));
			return false;
		}
		q->waitPut--;
		q->activePut = true;		
		if (q->size == 0){
			q->head = malloc(sizeof(tsqueue_node_t));
			if (q->head == NULL){
				PMUnlock(&(q->lock));
				return false;
			}
			q->head->elem = elem;
			q->head->next = NULL;
			q->tail = q->head;
			q->size = 1;
		} else {
			tsqueue_node_t* qn = malloc(sizeof(tsqueue_node_t));
			if (qn == NULL){
				PMUnlock(&(q->lock));			
				return false;
			}
			qn->elem = elem;
			q->tail->next = qn;
			q->tail = qn;
			q->size++;
		}
		q->activePut = false;
		/* TODO Is that okay?? */
		pthread_cond_signal(&(q->getVar));
		pthread_cond_signal(&(q->putVar));
		PMUnlock(&(q->lock));
	}
	return true;		
}

void* tsqueue_get(tsqueue_t* q){
	if (q == NULL){
		return NULL;
	} else {
		PMLock(&(q->lock));
		q->waitGet++;
		while (getShouldWait(q)){
			pthread_cond_wait(&(q->getVar), &(q->lock));
		}
		if (q->ended){
			PMUnlock(&(q->lock));
			return NULL;
		}
		q->waitGet--;
		q->activeGet = true;
		tsqueue_node_t* qn = q->head;
		q->head = qn->next;
		qn->next = NULL;
		q->size--;
		void* res = qn->elem;
		qn->elem = NULL;
		free(qn);
		q->activeGet = false;
		/* TODO Is that okay?? */
		pthread_cond_signal(&(q->putVar));
		pthread_cond_signal(&(q->getVar));
		PMUnlock(&(q->lock));
		return res;
	}
}

/* FIXME Sempre un problema distruggere le lock */
void** tsqueue_destroy(tsqueue_t* q){

	PMLock(&(q->lock));
	int n = tsqueue_size(q);
	void** res = NULL;
	res = calloc(n + 1, sizeof(void*));
	if (!res){
		PMUnlock(&(q->lock));
		return NULL;
	}
	PMUnlock(&(q->lock));

	for (int i = 0; i < n; i++) res[i] = tsqueue_get(q);
	
	PMLock(&(q->lock));
	q->ended = true;
	res[n] = NULL;
	pthread_cond_broadcast(&(q->putVar));
	pthread_cond_broadcast(&(q->getVar));
	pthread_yield(); /* Allows other put/get requests to terminate */

	pthread_cond_destroy(&(q->putVar));
	pthread_cond_destroy(&(q->getVar));
	PMUnlock(&(q->lock));

	pthread_mutex_destroy(&(q->lock));
	free(q);
	/* Se res == NULL, allora NON si può distruggere la coda in sicurezza */
	return res;
}

/**
 * @requires The last element of vect is NULL
 * @returns length of vect 
*/
int len(void** vect){
	if (vect == NULL) return -1;
	int res = 0;
	while (vect[res]) res++;
	return res;
}
