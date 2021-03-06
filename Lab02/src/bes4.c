//Versione migliorata dell'es.4 standard, con la possibilità di usare opzioni: -n <intero> -l <intero> -u <upper>

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

typedef enum {false, true} bool;


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

int recognize(int argc, char** argv, unsigned int* n, unsigned int* l, unsigned int* u){

	bool foundN = false;
	bool foundL = false;
	bool foundU = false;

	int opt;
	while ((opt = getopt(argc, argv, ":n:l:u:")) != -1){
		switch(opt){
			case '?':
				printf("Unrecognized option '-%c'\n", optopt); break;
			case ':':
				printf("No argument provided for option '-%c'\n", opt); break;
			case 'n':
				if (!foundN){
					foundN = true;
					int w = atoi(optarg);
					if (w < 0){fprintf(stderr, NERROR); return -1; }
					*n = (unsigned int)w;
				} else {
					printf("Error: duplicated option '-n'\n"); return -1;
				}
				break;
			case 'l':
				if (!foundL){
					foundL = true;
					int w = atoi(optarg);
					if (w < 0){fprintf(stderr, K1ERROR); return -1; }
					*l = (unsigned int)w;
				} else {
					printf("Error: duplicated option '-l'\n"); return -1;
				}
				break;
			case 'u':
				if (!foundU){
					foundU = true;
					int w = atoi(optarg);
					if (w < 0){fprintf(stderr, K2ERROR); return -1; }
					*u = (unsigned int)w;
				} else {
					printf("Error: duplicated option '-u'\n"); return -1;
				}
				break;
		}
	}
	if (!foundN) *n = N;
	if (!foundL) *l = K1;
	if (!foundU) *u = K2;
	return 0;
}

//Main
int main(int argc, char** argv){
	if (recognize(argc, argv, &n, &k1, &k2) == -1) return -1;
	return printRandoms(n, k1, k2);
}
