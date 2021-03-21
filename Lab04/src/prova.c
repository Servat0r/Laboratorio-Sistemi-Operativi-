#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 100

int main(void){
	int* M = malloc(N * sizeof(int));
	for (int i = 0; i < N/2; i++){
		M[i] = i;
	}
	for (int j = N/2; j < N; j++){
		M[j] = N;
	}

	int* K = malloc(N * sizeof(int));
	for (int i = 0; i < N/2; i++){
		K[i] = i;
	}
	for (int j = N/2; j < N; j++){
		K[j] = N/2;
	}
	printf("%d\n", memcmp(M, K, N*sizeof(int)));
	printf("%lu\n", sizeof(*M));
	return 0;
}
