#if !defined(_MYBOUNDEDQUEUE_H)
#define _MYBOUNDEDQUEUE_H

#include <stdbool.h>
#include <myqueue.h>

typedef struct BoundedQueue {
	Queue* q;
	int n;
} BoundedQueue;

BoundedQueue* initBQueue(int n);

bool bq_isEmpty(BoundedQueue bq);

bool bq_isFull(BoundedQueue bq);

size_t bq_size(BoundedQueue bq);

bool bq_enqueue(BoundedQueue* bq, void* elem);

void* bq_dequeue(BoundedQueue* bq);

#endif
