//Coda FIFO come LinkedList
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <myqueue.h> //Path da specificare come -I <...>

typedef enum {false, true} bool;

typedef struct queue_node {
	void* elem;
	struct queue_node* next;
} QueueNode;

typedef struct queue {
	QueueNode* head;
	QueueNode* tail;
	size_t size;
} Queue;

Queue* initQueue(void){
	Queue* q = malloc(sizeof(Queue));
	if (q == NULL) return NULL;
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

bool isEmpty(Queue q){
	return (q.size == 0 ? true : false);
}

size_t size(Queue q){
	return q.size;
}

bool enqueue(Queue* q, void* elem){
	if (q == NULL) {
		q = initQueue();
		if (q == NULL) return false;
		return enqueue(q, elem);
	} else if (q->size == 0){
		q->head = malloc(sizeof(QueueNode));
		if (q->head == NULL) return false;
		q->head->elem = elem;
		q->head->next = NULL;
		q->tail = q->head;
		q->size = 1;
	} else {
		QueueNode* qn = malloc(sizeof(QueueNode));
		if (qn == NULL) return false;
		qn->elem = elem;
		q->tail->next = qn;
		q->tail = qn;
		q->size++;
	}
	return true;		
}

void* dequeue(Queue* q){
	if (q == NULL){
		fprintf(stderr, "Error: invalid pointer!\n");
		errno = 1;
		return NULL;
	} else if (q->size == 0){
		fprintf(stderr, "Error: empty queue!\n");
		errno = 0;
		return NULL;
	} else {
		QueueNode* qn = q->head;
		q->head = qn->next;
		qn->next = NULL;
		q->size--;
		void* res = qn->elem;
		qn->elem = NULL;
		free(qn);
		return res;
	}
}

void** destroyQueue(Queue* q){
	int n = size(*q);
	void** res = NULL;
	if (n > 0){
		res = calloc(n + 1, sizeof(void*));
		for (int i = 0; i < n; i++) res[i] = dequeue(q);
		res[n] = NULL;
	}
	free(q);
	return res;
}

void showQueue(Queue* q){
	if (size(*q) > 0){
		QueueNode* head = q->head;
		while (head != NULL){
			printf("%s -> ", head->elem);
			head = head->next;
		}
		printf("\n");
	} else {
		printf("Empty queue\n");
	}
}
