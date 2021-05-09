#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "numfuncs.h"

#define PREC 16 //Floating-point cyphers output
#define NO_MORE_MEMORY "Error: no more memory available to continue execution!\n"
#define CANNOT_OPEN_TXT "Error: cannot open file './mat_dump.txt'\n"
#define CANNOT_OPEN_DAT "Error: cannot open file './mat_dump.dat'\n"

typedef int (*cmp_t)(const void*, const void*, size_t); //Tipo delle funzioni di comparazione (fra cui memcmp)

bool compare(cmp_t func, void* p, void* q, size_t memsize){
	if (sizeof(p) != sizeof(q)){
		fprintf(stderr, "Error: elements in the two memory spaces are NOT of the same size!\n");
		exit(1);
	}
	//printf("Comparing %lu bytes...\n", r);
	return ((*func)(p,q,memsize) == 0 ? true : false);
}

bool check_not_null(void* ptr, char* error_message){
	if (ptr == NULL){
		fprintf(stderr, error_message);
		return false;
	}
	return true;
}

//Stampa una matrice di float di dimensione N * N
void printMatrix(float* M, long N){
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			printf("%f ", *(M + i*N + j));
		}
		printf("\n");
	}
}

/* ----------------------------------------------------------------------------- */
int main(int argc, char* argv[]){
	if (argc < 2){
		fprintf(stderr, "Error: must give matrix size (a positive integer)!\n");
		return -1;
	}
	long N;
	if (!getInt(argv[1], &N) || (N <= 0)){ //Here we use lazy evaluation
		fprintf(stderr, "Error: matrix size must be a positive integer!\n");
		return -1;
	}
	float* M = malloc(N * N * sizeof(float)); //Allocated matrix
	if (!check_not_null(M, NO_MORE_MEMORY)) return 1;
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			*(M + i * N + j) = (i + j)/2.0;
		}
	}

	FILE* textual = fopen("./mat_dump.txt", "w");
	if (!check_not_null(textual, CANNOT_OPEN_TXT)) return 1;

	FILE* binary = fopen("./mat_dump.dat", "wb");
	if (!check_not_null(binary, CANNOT_OPEN_DAT)) return 1;

	//Writing on .txt file
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			fprintf(textual, "%f ", *(M+i*N+j));
		}
		fprintf(textual,"\n"); //newline at the end of each row of the matrix
	}
	fclose(textual);

	//Writing on .dat file
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			fwrite(M+i*N+j, sizeof(float), 1, binary);
		} //no newlines !
	}
	fclose(binary);

	//Free initial matrix
	free(M);

	//Now we read the two matrices
	float* M1 = malloc(N * N * sizeof(float));
	if (!check_not_null(M1, NO_MORE_MEMORY)) return 1;	

	FILE* textual_out = fopen("./mat_dump.txt", "r");
	if (!check_not_null(textual_out, CANNOT_OPEN_TXT)) {
		free(M1); return 1;
	}

	float* M2 = malloc(N * N * sizeof(float));
	if (!check_not_null(M2, NO_MORE_MEMORY)){
		free(M1); fclose(textual_out); return 1;
	}

	FILE* binary_out = fopen("./mat_dump.dat", "rb");
	if (!check_not_null(binary_out, CANNOT_OPEN_DAT)){
		free(M1); free(M2); fclose(textual_out); return 1;
	}


	//Here we read the "textual matrix"
	int txt_size = N*((PREC) + 1) + 1;

	char* input_line = malloc(txt_size*sizeof(char)); //Dovrebbe essere la giusta dimensione per contenere anche la '\n' ogni volta 
	if (!check_not_null(input_line, NO_MORE_MEMORY)) return 1;

	errno = 0;

	for (int i = 0; i < N; i++){
		if (fgets(input_line, txt_size, textual_out) == NULL){
			fprintf(stderr, "Error: uncorrect argument passed as matrix dimension or corrupted data in './mat_dump.txt'\n");
			errno = 2;
			break;
		} else {
			char* begin = input_line;
			char* end = input_line;
			for (int j = 0; j < N; j++){
				end = strchr(begin, ' ');
				if (end == NULL){
					fprintf(stderr, "Error: uncorrect argument passed as matrix dimension or corrupted data in './mat_dump.txt'\n");
					errno = 2;
					break;
				}
				end[0] = '\0';
				if (!getFloat(begin, M1 + i*N + j)){
					fprintf(stderr, "Error: uncorrect argument passed as matrix dimension or corrupted data in './mat_dump.txt'\n");
					errno = 2;
					break;				
				}
				begin = end + 1; //Si passa al prossimo numero
			}
			if (errno != 0) break;
		}
	}

	if (errno == 0){
		//Here we read the "binary matrix"
		size_t binary_res = fread(M2, sizeof(float), N*N, binary_out);
		if (binary_res != N*N){
			fprintf(stderr, "Error: uncorrect argument passed as matrix dimension or corrupted data in './mat_dump.dat'\n");
			errno = 2;
		}
	}

	#ifdef DEBUG_MODE
	if (errno == 0){
		printf("This is the output of the file './mat_dump.txt'\n");
		printMatrix(M1,N);
		printf("This is the output of the file './mat_dump.dat'\n");
		printMatrix(M2,N);
	}	
	#endif

	if (errno == 0){
		errno = (compare(&memcmp, M1, M2, N*N*sizeof(float)) == true ? 0 : 1);
		if (errno == 0) printf("Success: the two matrices ARE equal!\n");
		else if (errno == 1) fprintf(stderr, "Error: the two matrices are NOT equal!\n");
	}

	//Closing "textual" resources
	fclose(textual_out);
	free(input_line);
	free(M1);
	//Closing "binary" resources
	fclose(binary_out);
	free(M2);

	return errno;
}
