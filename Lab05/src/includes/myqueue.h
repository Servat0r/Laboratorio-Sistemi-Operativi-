#if !defined(_MYQUEUE_H)
#define _MYQUEUE_H

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

Queue* initQueue(void);

bool isEmpty(Queue q);

size_t size(Queue q);

bool enqueue(Queue* q, void* elem);

void* dequeue(Queue* q);

#endif
