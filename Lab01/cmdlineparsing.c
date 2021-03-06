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
//'m', opt, &foundM, waitState (stato in cui posizionarsi), index, &M, &returnCode
void handleNumOption(char name, char* opt, bool* found, int waitState, int index, long* value, int* retCode){ //void o int (returnCode) ? 
	if (strlen(opt) == 1){
		if (!*found){
			state = waitState;
		} else {
			fprintf(stderr, "Duplicated option: '-%c'\n [%d]", name, index);
			*retCode = 1;
		}
	} else {
		if (isNumber(opt+1)){
			if (!*found){
				long l = strtol(opt+1,NULL,10);
				*value = l;
				*found = true;
			} else {
				fprintf(stderr, "Duplicated option: '-n'\n [%d]", index);
				*retCode = 1;
			}
		} else {
			fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
			*retCode = 1;
		}
	}										
}

int main(int argc, char** argv){
	int returnCode = 0;
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
						if (strlen(opt) == 1){
							if (!foundN){
								state = 1;
							} else {
								fprintf(stderr, "Duplicated option: '-n'\n [%d]", index);
								returnCode = 1;
							}
						} else {
							if (isNumber(opt+1)){ //IS a number
								if (!foundN){
									N = strtol(opt+1,NULL,10);
									foundN = true;
								} else {
									fprintf(stderr, "Duplicated option: '-n'\n [%d]", index);
									returnCode = 1;
								}
							} else {
								fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
								returnCode = 1;
							}
						}
					} else if (opt[0] == 'm'){ 
						if (strlen(opt) == 1){
							if (!foundM){
								state = 2;
							} else {
								fprintf(stderr, "Duplicated option: '-m'\n [%d]", index);
								returnCode = 1;
							}
						} else {
							if (isNumber(opt+1)){ //IS a number
								if (!foundM){
									M = strtol(opt+1,NULL,10);
									foundM = true; //strtol(opt+1,NULL,10);
								} else {
									fprintf(stderr, "Duplicated option: '-m'\n [%d]", index);
									returnCode = 1;
								}
							} else {
								fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
								returnCode = 1;
							}
						}
					} else if (opt[0] == 's'){
						if (S != NULL){
							fprintf(stderr, "Duplicated option: '-s'\n [%d]", index);
						} else if (strlen(opt) == 1){
							state = 3;
						} else {
							S = opt + 1;
						}	
					} else if (opt[0] == 'h'){
						if (strlen(opt) > 1){
							fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
						} else if (!H){
							H = true;
						} else {
							fprintf(stderr, "Duplicated option: '-h'\n [%d]", index);
							returnCode = 1;
						}
					} else {
						fprintf(stderr, "Unrecognized option: '-%s'\n [%d]", opt, index);
						returnCode = 1;
					}  
				}
				//Else continue (ignore not expected argument)
			} else if (state == 1){
				state = 0;
				if (isNumber(word)){
					N = strtol(word, NULL, 10);					
					foundN = true;
				} else {
					fprintf(stderr, "Wrong argument for option '-n'\n [%d]", index);
					returnCode = 1;
				}
			} else if (state == 2){
				state = 0;
				if (isNumber(word)){					
					M = strtol(word, NULL, 10);					
					foundM = true;
				} else {
					fprintf(stderr, "Wrong argument for option '-m'\n [%d]", index);
					returnCode = 1;
				}		
			} else if (state == 3){
				state = 0;
				if (isOption(word) != NULL){
					fprintf(stderr, "No argument provided for option '-s'\n [%d]", index);
					fprintf(stderr, "Unrecognized option '%s'\n [%d]", word, index);
					returnCode = 1;
				} else {
					S = word;
				}
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
