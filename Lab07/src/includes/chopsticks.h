#if !defined(_CHOPSTICKS_H)
#define _CHOPSTICKS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct chopsticks_s {
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	pthread_barrier_t barrier;
	bool* busy;
	size_t size;
} chopsticks_t;

chopsticks_t*
	chopsticks_init(size_t);

bool
	chopsticks_ask(chopsticks_t*, int),
	chopsticks_release(chopsticks_t*, int);

void 
	chopsticks_destroy(chopsticks_t*),
	chopsticks_abandon(chopsticks_t*);

#endif
