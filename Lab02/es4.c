//Input dati: <program_path> [N [K1 [K2]]]

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define N 1000
#define K1 100
#define K2 120

#define NERROR "Error: n must be >0 !\n"
#define K1ERROR "Error: k1 must be >0 !\n"
#define K2ERROR "Error: k2 must be >0 !\n"
#define RANGERROR "Error: must have k1 < k2 !\n"


unsigned int n, k1, k2;

//Trasformata in funzione per renderla più safe
int printRandoms(unsigned int n, unsigned int k1, unsigned int k2){
	if (k2 <= k1){ fprintf(stderr, RANGERROR); return -1; }
	if (n <= 0){ fprintf(stderr, NERROR); return -1; }
	unsigned int seed = time(0);
	unsigned int* counts = calloc(k2-k1, sizeof(unsigned int)); //Un vettore di k2-k1 elementi per tenere il conto di quante volte esce ogni elemento
	if (counts == NULL) {fprintf(stderr,"Error: not enough memory available\n"); return -1; }	
	for (int i = 0; i < n; i++){
		counts[rand_r(&seed) % (k2-k1)] += (unsigned int)1;
	}
	for (int i = 0; i < k2-k1; i++) counts[i] *= 100; //Tutti moltiplicati per 100 (per fare le percentuali)
	double db; //Usato per tenere le percentuali
	printf("Occorrenze di:\n");
	for (int i = 0; i < k2-k1; i++){
		db = (double)counts[i] / (double)n;
		printf("%d : %.2f%%\n", k1 + i, db);
	}
	free(counts);
	return 0;
}

int main(int argc, char** argv){
	if (argc < 4){
		k2 = K2;
	} else {
		int w = atoi(argv[3]);
		if (w < 0){ fprintf(stderr, K2ERROR); return -1; }
		k2 = (unsigned int)w;
	}
	if (argc < 3){
		k1 = K1;
	} else {
		int w = atoi(argv[2]);
		if (w < 0){ fprintf(stderr, K1ERROR); return -1; }
		k1 = (unsigned int)w;
	}
	if (argc < 2){
		n = N;
	} else {
		int w = atoi(argv[1]);
		if (w < 0){ fprintf(stderr, NERROR); return -1; }
		n = (unsigned int)w;
	}
	return printRandoms(n, k1, k2);
}
