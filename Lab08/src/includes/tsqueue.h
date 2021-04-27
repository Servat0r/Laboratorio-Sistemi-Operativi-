/* 
 * FIXME Ma la coda è FIFO? Cioè dobbiamo gestire le put/get in ordine di arrivo?
 * Dobbiamo usare una priorità per i put/get-Threads in attesa??
 * (Rischio che i consumatori debbano ASPETTARE che i produttori finiscano!)
 * Il messaggio di terminazione fa terminare subito i consumatori, o all'inizio
 * del ciclo successivo (ad es.)? Aspetta che i consumatori svuotino la coda o no?
*/

#if !defined(_TSQUEUE_H)
#define _TSQUEUE_H

#include <stdbool.h>
#include <pthread.h>

typedef struct tsqueue_node {
	void* elem;
	struct tsqueue_node* next;
} tsqueue_node_t;


typedef struct tsqueue_s {
	tsqueue_node_t* head;
	tsqueue_node_t* tail;
	pthread_mutex_t lock;
	pthread_cond_t putVar;
	pthread_cond_t getVar;
	size_t size;
	unsigned int waitPut;
	unsigned int waitGet;
	bool activePut;
	bool activeGet;
	bool ended;
} tsqueue_t;


tsqueue_t* tsqueue_init(void);

bool tsqueue_put(tsqueue_t*, void*);

void* tsqueue_get(tsqueue_t*);

void** tsqueue_destroy(tsqueue_t*);

int len(void**);

#endif
