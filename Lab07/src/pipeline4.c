#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <myqueue.h>
#include <pthread.h>
#include <PMut.h>
#include <icl_hash.h>

#define MAX_LINE_LENGTH 4096 //4KB
#define NBUCKETS 10

/* Mutexes rispettivamente per linestream e tokenstream */
pthread_mutex_t mtx_line = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_tok = PTHREAD_MUTEX_INITIALIZER;

/* CondVars rispettivamente per linestream e tokenstream */
pthread_cond_t condline = PTHREAD_COND_INITIALIZER; 
pthread_cond_t condtok = PTHREAD_COND_INITIALIZER; 

/* Guardie per la validità dei token (una sorta di semafori) */
bool endRead = false;
bool endTok = false;
bool endWrite = false;


void tokenizer(char* buffer, Queue* q){
	char* tmpstr;
	char* token = strtok_r(buffer, " ", &tmpstr);
	while (token){
		char* tok = malloc((strlen(token) + 1) * sizeof(char));
		strncpy(tok, token, strlen(token) + 1);
		enqueue(q, tok);
		token = strtok_r(NULL, " ", &tmpstr);
	}
}

/* Thread che legge ogni riga dal file di input
@param arg : void*[2], dove arg[0] : FILE* è il file in input da tokenizzare e
	arg[1] : Queue* è la coda in cui inserire le righe lette 
*/
void* threadRead(void* arg){
	FILE* f = ((void**)arg)[0];
	Queue* linestream = ((void**)arg)[1];
	while (!endRead){
		PMLock(&mtx_line);
		char line[MAX_LINE_LENGTH];
		while ((size(*linestream) > 0) && !endRead) pthread_cond_wait(&condline, &mtx_line);
		if (fgets(line,MAX_LINE_LENGTH,f) == NULL){
			endRead = true;
		} else {
			enqueue(linestream, line);
		}
		pthread_cond_signal(&condline);
		PMUnlock(&mtx_line);
	}
	fclose(f);
	return NULL;
}

/* Thread che tokenizza ogni riga che riceve
@param arg : void**, dove arg[0], arg[1] : Queue* sono rispettivamente la coda
in cui threadRead inserisce le righe lette e quella in cui threadTok inserisce
i token di ogni riga
*/
void* threadTok(void* arg){
	Queue* linestream = ((void**)arg)[0];
	Queue* tokenstream = ((void**)arg)[1];
	while (!endTok){
		char buffer[MAX_LINE_LENGTH];
		PMLock(&mtx_line);
		while((size(*linestream) == 0) && !endRead) pthread_cond_wait(&condline, &mtx_line);
		if (endRead && (size(*linestream) == 0)){
			endTok = true;
			pthread_cond_signal(&condtok);
			PMUnlock(&mtx_line);
		} else {
			strncpy(buffer, dequeue(linestream), MAX_LINE_LENGTH);
			//printf("buffer = %s\n", buffer); fflush(stdout); //showQueue(linestream); showQueue(tokenstream);
			pthread_cond_signal(&condline);	
			PMUnlock(&mtx_line);

			PMLock(&mtx_tok);
			while ((size(*tokenstream) > 0) && !endTok) pthread_cond_wait(&condtok, &mtx_tok); //Serve !endTok ?
			tokenizer(buffer, tokenstream);
			//TODO Anche la tokenizzazione si può mettere fuori (si crea un array di tokens che vengono poi caricati su tokenstream in ME)
			pthread_cond_signal(&condtok);
			PMUnlock(&mtx_tok);
		}
	}
	return NULL;
}

/* Thread che stampa ogni stringa che riceve 
@param arg : void**, dove arg[0] : Queue* è la coda da cui threadWrite estrae
i token
*/
void* threadWrite(void* arg){
	Queue* tokenstream = ((void**)arg)[0];
	char* token;
	icl_hash_t* hashTable = icl_hash_create(NBUCKETS, &hash_pjw, &string_compare);
	while (!endWrite){
		PMLock(&mtx_tok);
		while ((size(*tokenstream) == 0) && !endTok) pthread_cond_wait(&condtok, &mtx_tok);
		//Viene risvegliato dalla signal		
		if (endTok && (size(*tokenstream) == 0)){
			endWrite = true;
			pthread_cond_signal(&condtok);
			PMUnlock(&mtx_tok);
		} else { 
			while (size(*tokenstream) > 0){
				token = (char*)(dequeue(tokenstream));
				if (!icl_hash_insert(hashTable, token, NULL)) free(token);
			}
			pthread_cond_signal(&condtok);
			PMUnlock(&mtx_tok);
		}
	}
	int retVal1 = icl_hash_printKeys(stdout, hashTable);
	int retVal2 = icl_hash_destroy(hashTable, &free, NULL);
	printf("threadWrite returned: %d\n", retVal1 | retVal2);
	return NULL;
}

int main(int argc, char* argv[]){

	if (argc < 2){
		printf("Usage: %s filename\n", argv[0]);
		return -1;
	}

	FILE* inputfile;
	if ((inputfile = fopen(argv[1], "r")) == NULL){
		perror("fopen");
		return -1;
	}

	pthread_t tid1, tid2, tid3;

	Queue* linestream = initQueue();
	Queue* tokenstream = initQueue();

	void* argRead[2];
	void* argTok[2];
	void* argWrite[1];

	argRead[0] = inputfile;
	argRead[1] = linestream;

	argTok[0] = linestream;
	argTok[1] = tokenstream;

	argWrite[0] = tokenstream;

	pthread_create(&tid1,NULL,&threadRead,argRead);
	pthread_create(&tid2,NULL,&threadTok,argTok);
	pthread_create(&tid3,NULL,&threadWrite,argWrite);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	printf("Done\n");

	void** remline = destroyQueue(linestream); //Non contiene strutture heap-allocated
	free(remline);
	
	void** remtok = destroyQueue(tokenstream);
	if (remtok != NULL){
		int k = 0;
		while (remtok[k]) free(remtok[k]);
		free(remtok);
	}
	return 0;
}
