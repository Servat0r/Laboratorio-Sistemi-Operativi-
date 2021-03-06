/*

(state, N, M, S, H) con:

state
0 -> waiting for (-n<arg>) / (-m<arg>) / (-s<arg>) / (-h) OR closed
1 -> waiting for (-n) option argument (CAN be a negative integer !)
2 -> waiting for (-m) option argument (CAN be a negative integer !)
3 -> waiting for (-s) option argument (MUST NOT start with '-' !)

N,M : int* ; S : char* ; H : bool

Failing cases (when they happen):

1) Unrecognized option ()

2) Duplicated option ()

3) No argument for option -n/-m/-s

4) Wrong argument for option -n/-m/-s
 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum {false, true} bool; //For H

int state = 0;
bool foundN = false;
bool foundM = false;
long N = 0;
long M = 0;
char* S = NULL;
bool H = false;

/*
Requires: str != NULL
Return: The name of the option (e.g. -Werror => Werror, -Werror-longobardo => Werror-longobardo) if recognized, NULL otherwise.
Regex is: (-)+(.)+
*/
char* isOption(char* str){
	if ((str == NULL) || (str[0] != '-')) return NULL; //len == 0 OR NOT starts with '-'
	char* next = str + 1;
	while (next != NULL){
		if (next[0] != '-'){
			return next;
		} else {
			next += 1;
		}
	}
	return NULL;
}

bool isNumber(char* str){
	if (str == NULL) return false; //MUST check !
	bool result = false;
	int current = 0;
	int length = strlen(str);
	if (str[0] == '-') current = 1;
	while (current < length){
		if (str[current] >= '0' && str[current] <= '9'){
			result = true;
			current++;
		} else {
			result = false;
			break;
		}
	}
	return result;
}

/*
@param name Nome dell'opzione (1 carattere !)
@param opt Stringa da analizzare che per ipotesi è t.c. (opt[0] == name)
@param found Parametro che indica se questa opzione è già stata trovata (foundN, foundM)
@param state Stato globale attuale
@param waitState Stato in cui transire se è fornita solo l'opzione in opt
@param index Indice del 'token' corrente
@param value Valore (eventualmente) da assegnare come argomento dell'opzione (se presente)
@param retCode Codice di ritorno per il main che l'handling fornisce
*/
int handleNumOption(char name, char* opt, bool* found, int* state, int waitState, int index, long* value){ //void o int (returnCode) ?
	if (*state == 0){ 
		if (strlen(opt) == 1){
			if (!*found){
				*state = waitState;
				return 0;
			} else {
				fprintf(stderr, "Duplicated option: '-%c'\n [%d]", name, index);
				return 1;
			}
		} else {
			if (isNumber(opt+1)){
				if (!*found){
					long l = strtol(opt+1,NULL,10);
					*value = l;
					*found = true;
					return 0;
				} else {
					fprintf(stderr, "Duplicated option: '-n'\n [%d]", index);
					return 1;
				}
			} else {
				fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
				return 1;
			}
		}
	} else {
		*state = 0;
		if (isNumber(opt)){
			long l = strtol(opt, NULL, 10);
			*value = l;					
			*found = true;
			return 0;
		} else {
			fprintf(stderr, "Wrong argument for option '-%c'\n [%d]", name, index);
			return 1;
		}
	}										
}

int handleStringOption(char name, char* opt, bool* found, int* state, int waitState, int index, char** value){
	if (*state == 0){
		if (*value != NULL){
			fprintf(stderr, "Duplicated option: '-%c'\n [%d]", name, index);
			return 1;
		} else if (strlen(opt) == 1){
			*state = 3;
			return 0;
		} else {
			*value = opt + 1;
			return 0;
		}
	} else {
		*state = 0;
		if (isOption(opt) != NULL){
			fprintf(stderr, "No argument provided for option '-%c'\n [%d]", name, index);
			return 1;
		} else {
			*value = opt;
			return 0;
		}
	}
}

int handleHelpOption(char name, char* opt, bool* found, int* state, int waitState, int index, bool* value){
	if (*state == 0){
		if (strlen(opt) > 1){
			fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
			return 0;
		} else if (!*value){
			*value = true;
			return 0;
		} else {
			fprintf(stderr, "Duplicated option: '-%c'\n [%d]", name, index);
			return 1;
		}
	}
}

int cmdLineParsing(int argc, char** argv){
	int returnCode = 0;
	int actualCode = 0;
	if (argc < 2){
		fprintf(stderr, "No arguments provided\n");
		return -1;
	} else {
		char* word;
		for (int index = 1; index < argc; index++){
			word = argv[index];
			if (state == 0){
				char* opt = isOption(word);
				if (opt != NULL){
					if (opt[0] == 'n'){
						actualCode = handleNumOption('n', opt, &foundN, &state, 1, index, &N);
						returnCode = (returnCode == 0 ? actualCode : 1);
					} else if (opt[0] == 'm'){ 
						actualCode = handleNumOption('m', opt, &foundM, &state, 2, index, &M);
						returnCode = (returnCode == 0 ? actualCode : 1); 
					} else if (opt[0] == 's'){
						actualCode = handleStringOption('s', opt, NULL, &state, 3, index, &S);
						returnCode = (returnCode == 0 ? actualCode : 1);
					} else if (opt[0] == 'h'){
						actualCode = handleHelpOption('h', opt, NULL, &state, 0, index, &H);
						returnCode = (returnCode == 0 ? actualCode : 1);
					} else {
						fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
						returnCode = 1;
					}  
				}
				//Else continue (ignore not expected argument)
			} else if (state == 1){
				actualCode = handleNumOption('n', word, &foundN, &state, 1, index, &N);
				returnCode = (returnCode == 0 ? actualCode : 1);
			} else if (state == 2){
				actualCode = handleNumOption('m', word, &foundM, &state, 2, index, &M);
				returnCode = (returnCode == 0 ? actualCode : 1);
			} else if (state == 3){
				actualCode = handleStringOption('s', word, NULL, &state, 3, index, &S);
				returnCode = (returnCode == 0 ? actualCode : 1);
			}	
		}
	}
	if (H){
		printf("nome-programma -n <numero> -s <stringa> -m <numero> -h\n");
	} else {
		printf("%s ", argv[0]); //Nome programma
		if (foundN){
			printf("-n: %ld ", N);
		}
		if (S != NULL){
			printf("-s: %s ", S);
		}
		if (foundM){
			printf("-m: %ld ", M);
		}
		printf("\n");
	}
	return returnCode;
}

int main(int argc, char** argv){ return cmdLineParsing(argc, argv); }

/*
This should be a much more generalized version:

typedef struct Option {
	char* name;
	unsigned int howManyMinus;
	bool found;
	char* value;
	<functionPointer> isCorrectValue;
	<functionPointer> handler;
	<functionPointer> converter;
} option;
*/
