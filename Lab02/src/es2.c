#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef enum {false, true} bool;
typedef int opt_t(const char*);

bool isNumber(const char* str){
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

bool foundN = false;
bool foundM = false;
bool foundO = false;

long m = 0;
long n = 0;

bool validN = false;
bool validM = false;

bool H = false;

int arg_n(const char* arg){
	int ret = 0;
	if (!foundN){
		foundN = true;
		if (isNumber(arg)){ 
			validN = true; 
			printf("-n = %ld\n", strtol(optarg, NULL, 10)); 
		} //A questo punto siam sicuri che sia DAVVERO un numero
		else { 
			optind--; 
			printf("Error: none or invalid argument for option '-n'\n"); 
			ret = -1;
		}
	} else {
		printf("Error: duplicated option '-n'\n"); ret = -1;
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
	return ret;
}

int arg_m(const char* arg){
	int ret = 0;
	if (!foundM){
		foundM = true;
		if (isNumber(arg)){ 
			validM = true; 
			printf("-m = %ld\n", strtol(optarg, NULL, 10));
		}
		else { 
			optind--; 
			printf("Error: none or invalid argument for option '-m'\n"); 
			ret = -1;}
	} else {
		printf("Error: duplicated option '-m'\n"); ret = -1;
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
	return ret;
}

int arg_o(const char* arg){
	int ret = 0;
	if (!foundO){
		foundO = true;
		if ((arg[0] == '-') && ((arg[1] == 'n') || (arg[1] == 'm') || (arg[1] == 'o') || (arg[1] == 'h'))){
			printf("Error: no argument provided for option '-o'\n");
			ret = -1;
			optind--;
		} else {
			printf("-o = %s\n",arg);
		}
	} else {
		printf("Error: duplicated option '-o'\n"); ret = -1;
	}
	#ifdef DEBUG_MODE
		printf("(case o)\n");
		printf("opt = %c\n", opt);
		printf("optopt = %c\n", optopt);
		printf("optarg = %s\n", optarg);
		printf("optind = %d\n", optind);
		printf("foundO = %d\n", foundO);
	#endif
	return ret;
}

int arg_h(const char* arg){
	if (!H){
		printf("Usage:\n<nome-programma> -n <intero> -m <intero> -o <stringa> -h\n");
		return 0;
	} else {
		printf("Error: duplicated option '-h'\n");
		return -1;
	}
}


int main(int argc, char* argv[]){
	
	if (argc < 2){ printf("No arguments provided\n"); return -1; }

	opt_t* V[4] = {&arg_h, &arg_m, &arg_n, &arg_o};

	int opt;

	int r = EXIT_SUCCESS;

	while ((opt = getopt(argc, argv, ":n:m:o:h")) != -1){
		switch(opt){
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
				r = EXIT_FAILURE;				
				break;
			case '?': {
				printf("Error: unknown option: -%c\n", optopt); 
				r = EXIT_FAILURE; 
				break;
			}
			default:
				if ((*V[opt%4])((optarg == NULL ? argv[0] : optarg)) == -1){
					r = EXIT_FAILURE;	
				}
		}
	}
	return r;
}
