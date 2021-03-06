//Esercizio 4
//Alert: INIT_VALUE is defined when compiling: 'gcc es4.c -DINIT_VALUE=100 -o es4'

//TODO: Realizzare la versione rientrante semplice (arity == 1), e thread-safe (arity > 1)

#include <stdio.h>
#include <stdlib.h>

//Non rientrante ma thread-safe
int somma(int x){
	static int s = INIT_VALUE;
	//Lock();
	s += x;
	//Unlock();
	return s;
}

//Versione rientrante
int somma_r(int x){
	static int s = INIT_VALUE;
	int temp = x + s;
	s = temp;
	return temp;
}

//Versione rientrante ma con un argomento in più
int somma_rts(int x, int s){
	return s + x;
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
