//Esercizio 4
//Alert: INIT_VALUE is defined when compiling: 'gcc es4.c -DINIT_VALUE=100 -o es4'


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifndef INIT_VALUE
	#error("Compilare con il flag -DINIT_VALUE=<integer>")
	exit(1);
#endif

//Non rientrante ma thread-safe
int somma(int x){
	static int s = INIT_VALUE;
	//Lock();
	s += x;
	//Unlock();
	return s;
}

//Versione rientrante
int somma_rts(int x, int* s){
	*s += x;
	return *s;
}

int main(int argc, char* argv[]){
	if (argc < 2) {fprintf(stderr, "Error: too few arguments\n"); exit(EXIT_FAILURE);}
	int n = atoi(argv[1]);
	int x; //Input
	for (int i = 0; i < n; i++){
		printf("Inserisci il prossimo valore: ");
		scanf("%d", &x);
		printf("Somma corrente: %d\n",somma(x));
	}
	return 0;
}
