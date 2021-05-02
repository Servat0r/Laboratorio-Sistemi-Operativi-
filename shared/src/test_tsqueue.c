#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tsqueue.h>
#include <pthread.h>
#include <PMut.h>

#define N 10
#define SEPARATOR "\n-----------------------------\n"

tsqueue_t* q; //Global variable

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;



void* putThread(void* arg){
	int k = *((int*)arg); //idPutThread
	//unsigned int seed = time(0);
	int* r;
	for (int i = 0; i < N; i++){
		r = calloc(2, sizeof(int));
		r[0] = k;
		r[1] = i;
		tsqueue_put(q, r);
		printf("Put [%d, %d]\n", r[0], r[1]);
	}
	return NULL;
}

void* getThread(void* arg){
	int k = *((int*)arg); //idGetThread
	int* r;
	for (int i = 0; i < N; i++){
		assert((r = tsqueue_get(q)) != NULL);
		printf("Thread %d has extracted [%d, %d]\n", k, r[0], r[1]);
		free(r);
	}
	return NULL;
}

int main(){
	
	/* Test1: one producer and one consumer */
	q = tsqueue_init();
	pthread_t test1[2];
	int putId = 1;
	int getId = 1;
	pthread_create(&test1[0], NULL, &putThread, &putId);
	pthread_create(&test1[1], NULL, &getThread, &getId);
	pthread_join(test1[0], NULL);
	pthread_join(test1[1], NULL);
	void** res = tsqueue_destroy(q); 	
	assert(len(res) == 0);
	free(res);
	printf("Test1 okay!\n");
	printf(SEPARATOR);
	/* Test1 */

	/* Test2: four producers and one consumer */
	q = tsqueue_init();
	pthread_t test2[5];
	int putId2[4] = {1,2,3,4};
	int getId2 = 1;
	for (int i = 0; i < 4; i++) pthread_create(&test2[i], NULL, &putThread, &putId2[i]);
	pthread_create(&test2[4], NULL, &getThread, &getId2);
	for (int i = 0; i < 4; i++) pthread_join(test2[i], NULL);
	pthread_join(test2[4], NULL);
	res = tsqueue_destroy(q);
	assert(len(res) == 30);
	for (int i = 0; i < 30; i++) free(res[i]);
	free(res);
	printf("Test2 okay\n");
	printf(SEPARATOR);
	/* Test2 */

	/* Test3: four producers and four consumers */
	q = tsqueue_init();
	pthread_t test3[8];
	int putId3[4] = {1,2,3,4};
	int getId3[4] = {1,2,3,4};
	for (int i = 0; i < 4; i++){
		pthread_create(&test3[i], NULL, &putThread, &putId3[i]);
		pthread_create(&test3[i+4], NULL, &getThread, &getId3[i]);
	}
	for (int i = 0; i < 8; i++) pthread_join(test3[i], NULL);
	res = tsqueue_destroy(q);
	assert(len(res) == 0);
	free(res);
	printf("Test3 okay\n");
	printf(SEPARATOR);
	/* Test3 */

	/* Test4: one producer and four consumers */
	q = tsqueue_init();
	pthread_t test4[5];
	int putId4 = 1;
	int getId4[4] = {1,2,3,4};
	pthread_create(&test4[0], NULL, &putThread, &putId4);
	for (int i = 1; i < 5; i++){
		pthread_create(&test4[i], NULL, &getThread, &getId4[i-1]);
	}
	pthread_join(test4[0], NULL);
	for (int i = 1; i < 5; i++) pthread_cancel(test4[i]);
	res = tsqueue_destroy(q);
	int l = len(res);
	for (int i = 0; i < l; i++) free(res[i]);
	free(res);
	printf("Test4 okay\n");
	printf(SEPARATOR);
	/* Test4 */

	return 0;					
}
