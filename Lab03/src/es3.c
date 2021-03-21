#include <stdio.h>
#include <stdlib.h>

// dimN = #righe, dimM = #colonne (!!)

#define dimN 16
#define dimM 8

#define CHECK_PTR_EXIT(X,Y) \
	if ((X) == NULL){ \
		perror(#Y); \
		exit(EXIT_FAILURE); \
	}

#define ELEM(M,i,j) \
	if ((i) >= dimN) { fprintf(stderr,"Error: index i = %d, out of range [0,%d]\n", ((int)i), dimN - 1); exit(EXIT_FAILURE);}; \
	if ((j) >= dimM) { fprintf(stderr,"Error: index j = %d, out of range [0,%d]\n", ((int)j), dimM - 1); exit(EXIT_FAILURE);}; \
	*((M) + (i)*dimM + (j))

#define PRINTMAT(M,X,Y) \
	for (size_t i = 0; i < (X); i++) { \
		for (size_t j = 0; j < (Y); j++){ \
			printf("%ld\t", *((M) + i*(Y) + j)); \
		} \
		printf("\n"); \
	}	

int main(void){
	long* M = malloc(dimM*dimN*sizeof(long));
	CHECK_PTR_EXIT(M,"malloc");
	for(size_t i = 0; i < dimN; ++i){
		for(size_t j = 0; j < dimM; ++j){
			ELEM(M,i,j) = i+j;
		}
	}
	PRINTMAT(M,dimN,dimM);
	free(M);
	return 0;
}
