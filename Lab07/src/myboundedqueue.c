#include <stdio.h>
#include <stdlib.h>
#include <myboundedqueue.h>

BoundedQueue* initBQueue(int n){
	if (n <= 0) return NULL;
	BoundedQueue* bq = malloc(sizeof(BoundedQueue));
	if (bq == NULL) return NULL;
	bq->q = initQueue();
	bq->n = n;
	return bq;
}

bool bq_isEmpty(BoundedQueue bq){
	return isEmpty(*(bq.q));
}

bool bq_isFull(BoundedQueue bq){
	return (size(*(bq.q)) >= bq.n ? true : false);
}

size_t bq_size(BoundedQueue bq){
	return size(*(bq.q));
}

bool bq_enqueue(BoundedQueue* bq, void* elem){
	return enqueue(bq->q, elem);
}

void* bq_dequeue(BoundedQueue* bq){
	return dequeue(bq->q);
}
