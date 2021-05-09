/* Esempio di uso di getopt */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

typedef enum {false, true} bool;

bool isNumber(char* str){
	if (str == NULL) return false; //MUST check !
	if (strlen(str) == 0) return false; //MUST check ! ##missing
	bool result = false;
	int current = 0;
	int length = strlen(str); // > 0 a questo punto
	if (str[0] == '-') current = 1;
	//Questo controlla già che sia (almeno in teoria) un numero
	while (current < length){
		if (str[current] >= '0' && str[current] <= '9'){
			result = true;
			current++;
		} else {
			result = false;
			break;
		}
	}
	if (result) {
		char* e = NULL;
		errno = 0;
		long val = strtol(str, &e, 10);
		if (errno == ERANGE) result = false; //Overflow
	}
	return result;
}

//Si possono anche aggiungere delle variabili per indicare "argomento N/M/O memorizzato" (ad esempio rendere N ed M dei long* anziché long)
bool foundN = false;
bool foundM = false;
bool foundO = false;

bool validN = false;
bool validM = false;

long N;
long M;
char* O;
bool H = false;


int main(int argc, char* argv[]){
	
	if (argc < 2){ printf("No arguments provided\n"); return -1; }

	int opt;

	int returnCode = 0;

	while ((opt = getopt(argc, argv, ":n:m:o:h")) != -1){
		//printf("optind = %d\n", optind);
		switch(opt){
			case 'n':
				if (!foundN){
					foundN = true;
					if (isNumber(optarg)){ validN = true; N = strtol(optarg, NULL, 10); } //A questo punto siam sicuri che sia DAVVERO un numero
					else { 
						optind--; 
						printf("Error: none or invalid argument for option '-n'\n"); 
						returnCode = 1;
					}
				} else {
					printf("Error: duplicated option '-n'\n"); returnCode = 1;
				}
				#ifdef DEBUG_MODE
					printf("(case n)\n");
					printf("opt = %c\n", opt);
					printf("optopt = %c\n", optopt);
					printf("optarg = %s\n", optarg);
					printf("optind = %d\n", optind);
					printf("foundN = %d\n", foundN);
					printf("validN = %d\n", validN);
				#endif
				break;
			case 'm':
				if (!foundM){
					foundM = true;
					if (isNumber(optarg)){ validM = true; M = strtol(optarg, NULL, 10); }
					else { optind--; printf("Error: none or invalid argument for option '-m'\n"); returnCode = 1;}
				} else {
					printf("Error: duplicated option '-m'\n"); returnCode = 1;
				}
				#ifdef DEBUG_MODE
					printf("(case m)\n");
					printf("opt = %c\n", opt);
					printf("optopt = %c\n", optopt);
					printf("optarg = %s\n", optarg);
					printf("optind = %d\n", optind);
					printf("foundM = %d\n", foundM);
					printf("validM = %d\n", validM);
				#endif
				break;
			case 'o':
				if (!foundO){
					foundO = true;
					if ((optarg[0] == '-') && ((optarg[1] == 'n') || (optarg[1] == 'm') || (optarg[1] == 'o') || (optarg[1] == 'h'))){
						printf("Error: no argument provided for option '-o'\n"); returnCode = 1;
						optind--;
					} else {
						O = optarg;
					}
				} else {
					printf("Error: duplicated option '-o'\n"); returnCode = 1;
				}
				#ifdef DEBUG_MODE
					printf("(case o)\n");
					printf("opt = %c\n", opt);
					printf("optopt = %c\n", optopt);
					printf("optarg = %s\n", optarg);
					printf("optind = %d\n", optind);
					printf("foundO = %d\n", foundO);
				#endif
				break;				
			case 'h':
				if (!H){
					H = true;
				} else {
					printf("Error: duplicated option '-h'\n"); returnCode = 1;
				}
				break;
			case ':':
				switch(optopt){
					case('n'):
						if (foundN){
							printf("Error: duplicated option '-n'\n");
						} else {
							foundN = true;
						}
						break;
					case('m'): 
						if (foundM){
							printf("Error: duplicated option '-m'\n");
						} else {
							foundM = true;
						}
						break;
					case('o'):
						if (foundO){
							printf("Error: duplicated option '-o'\n");
						} else {
							foundO = true;
						}
						break;
				}
				#ifdef DEBUG_MODE
					printf("(case ':')\n");
					printf("opt = %c\n", opt);
					printf("optopt = %c\n", optopt);
					printf("optarg = %s\n", optarg);
					printf("optind = %d\n", optind);
					printf("foundN = %d\n", foundN);
					printf("validN = %d\n", validN);
					printf("foundM = %d\n", foundM);
					printf("validM = %d\n", validM);
					printf("foundO = %d\n", foundO);
				#endif
				printf("Error: no argument provided for option '-%c'\n", optopt);				
				returnCode = 1; break;
			case '?':
				printf("Error: unknown option: -%c\n", optopt); returnCode = 1; break;
		}

	}
	if (H){
		printf("Usage:\n<nome-programma> -n <intero> -m <intero> -o <stringa> -h\n");
	} else {
		printf("%s ", argv[0]); //Nome programma
		if (foundN && validN) printf("-n %ld ", N);
		if (foundM && validM) printf("-m %ld ", M);
		if (foundO && (O != NULL)) printf("-o %s ", O);
		printf("\n");
	}
	#ifdef DEBUG_MODE
	printf("Ending...\n");
	printf("foundN = %d\n", foundN);
	printf("foundM = %d\n", foundM);
	printf("foundO = %d\n", foundO);
	printf("validN = %d\n", validN);
	printf("validM = %d\n", validM);
	#endif
	return returnCode;
}
