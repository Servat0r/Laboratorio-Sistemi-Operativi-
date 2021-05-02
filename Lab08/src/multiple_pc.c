/** 
 * Multiple producer-consumer queue example.
 *
 * @author Salvatore Correnti
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <PMut.h>
#include <tsqueue.h>
#include <assert.h>

/* Default values for m, n, k - we suppose that k % m == 0 */
#define M 3
#define N 4
#define K 15

/* Help string (showed with -h option or bad arguments). */
#define USAGE "Usage: %s [-m M] [-n N] [-k K] [-h], where M, N, K are positive \
integers and K is a multiple of M.\nDefault values are: M = 3; N = 4; K = 15. \
\nIf you do not provide all values, default values will be used for the remaining ones.\n"

#define OPT_STRING ":m:n:k:h"

int optopt;
char* optarg;

/* Structs for arguments to putThread and getThread */
typedef struct putArgs_s {
	int id;
	tsqueue_t* q;
	int numitems;
} putArgs_t;

typedef struct getArgs_s {
	int id;
	tsqueue_t* q;
} getArgs_t;

/** 
 * @param args : putArgs, such that
 * 	- args->id is the 'local' id of this thread;
 *	- args->q is the concurrent queue where to transmit messages;
 *	- args->numitems The number of items to produce.
 * @returns NULL.
*/
void* putThread(void* args){
	int id = ((putArgs_t*)args)->id;
	tsqueue_t* q = ((putArgs_t*)args)->q;
	if (q == NULL){ fprintf(stderr, "Error: null queue!\n"); return NULL; }
	int numitems = ((putArgs_t*)args)->numitems;	

	for (int i = 0; i < numitems; i++){
		int* r = calloc(2, sizeof(int));
		r[0] = id;
		r[1] = i;
		tsqueue_put(q, r);	
	}
	printf("putThread #%d exiting...\n", id);
	return NULL;
}

/** 
 * @param args : getArgs, such that
 * 	- args->id is the 'local' id of this thread;
 *	- args->q is the concurrent queue where to transmit messages.
 * @returns NULL.
*/
void* getThread(void* args){
	int id = ((getArgs_t*)args)->id;
	tsqueue_t* q = ((getArgs_t*)args)->q;
	if (q == NULL){ fprintf(stderr, "Error: null queue!\n"); return NULL; }
	int* res;
	while (true){
		res = tsqueue_get(q);
		if (res == (int*)NULL){
			break;
		}
		printf("thread #%d extracted [%d, %d]\n", id, res[0], res[1]);
		free(res); 
	}
	printf("getThread #%d exiting...\n", id);
	return NULL;
}


int main(int argc, char* argv[]){

	int m = M;
	int n = N;
	int k = K;
	int opt;
	tsqueue_t* q;
	while ((opt = getopt(argc, argv, OPT_STRING)) != -1){
		switch(opt){
			case '?': {
				fprintf(stderr, "Unrecognized option '-%c'\n", optopt);
				break;
			}
			case ':': {
				fprintf(stderr, "Option '-%c' needs argument\n", optopt);
				break;
			}
			case 'm': {
				m = atoi(optarg); break;
			}
			case 'n': {
				n = atoi(optarg); break;
			}
			case 'k': {
				k = atoi(optarg); break;
			}
			case 'h': {
				printf(USAGE, argv[0]); break;
			}
		}
	}

	if ((k <= 0) || (m <= 0) || (n <= 0)){
		fprintf(stderr, "Error: k, m, n MUST be > 0!\n");
		exit(EXIT_FAILURE);
	}

	if (k % m != 0){
		fprintf(stderr, "Error: k MUST be a multiple of m!\n");
		exit(EXIT_FAILURE);
	}

	int p = k/m; /* Number of items to produce for each putThread */

	/* tsqueue initialization */
	q = tsqueue_init();
	if (q == NULL){ perror("tsqueue_init"); exit(EXIT_FAILURE); }

	/* threadIds */	
	pthread_t putIds[m];
	pthread_t getIds[n];
	
	/* Arguments of putThreads */
	putArgs_t* putArgs[m];
	for (int i = 0; i < m; i++){
		putArgs[i] = malloc(sizeof(putArgs_t));
		putArgs[i]->id = i;
		putArgs[i]->q = q;
		putArgs[i]->numitems = p;
		pthread_create(&putIds[i], NULL, &putThread, putArgs[i]);
	}

	/* Arguments of getThreads */
	getArgs_t* getArgs[n];
	for (int i = 0; i < n; i++){
		getArgs[i] = malloc(sizeof(getArgs_t));
		getArgs[i]->id = i;
		getArgs[i]->q = q;
		pthread_create(&getIds[i], NULL, &getThread, getArgs[i]);
	}

	/* Waiting for putThreads */
	for (int i = 0; i < m; i++){
		pthread_join(putIds[i], NULL);
		free(putArgs[i]);
	}
	printf("Producers terminated\n");

	/* Termination messages */
	for (int h = 0; h < n; h++) tsqueue_put(q, (int*)NULL);

	for (int j = 0; j < n; j++){
		pthread_join(getIds[j], NULL);
		free(getArgs[j]);
	}
	printf("Consumers terminated\n");
	void** res = tsqueue_destroy(q);

	/* Test whether the queue was empty or not */
	assert(len(res) == 0);
	free(res);

	return 0;
}
