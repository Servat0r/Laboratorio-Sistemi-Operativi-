/*
Esempio di uso di getopt




*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef enum {false, true} bool;
typedef int opt_t(const char*);

bool isNumber(const char* str){
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

bool foundN = false;
bool foundM = false;
bool foundO = false;
bool H = false;

int arg_n(const char* arg){
	if (!foundN){
		foundN = true;
		if (isNumber(arg)){
			printf("-n = %ld\n", strtol(arg, NULL, 10));
			return 0;
		} else {
			optind--;
			printf("Error: none or invalid argument for option '-n'\n");
			return -1;
		}
	} else {
		printf("Error: duplicated option '-n'\n"); 
		return -1;
	}
}

int arg_m(const char* arg){
	if (!foundM){
		foundM = true;
		if (isNumber(arg)){
			printf("-m = %ld\n", strtol(arg, NULL, 10));
			return 0;
		} else {
			optind--;
			printf("Error: none or invalid argument for option '-m'\n");
			return -1;
		}
	} else {
		printf("Error: duplicated option '-m'\n"); 
		return -1;
	}
}

int arg_o(const char* arg){
	if (!foundO){
		foundO = true;
		if ((arg[0] == '-') && ((arg[1] == 'n') || (arg[1] == 'm') || (arg[1] == 'o') || (arg[1] == 'h'))){
			optind--;
			printf("Error: no argument provided for option '-o'\n");
			return -1;
		} else {
			printf("-o = %s\n", arg);
			return 0;
		}
	} else {
		printf("Error: duplicated option '-o'\n");
		return -1;
	}
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

	while ((opt = getopt(argc, argv, ":n:m:o:h")) != -1){
		switch(opt){
			case ':':
				printf("Error: no argument provided for option '-%c'\n", optopt); break;
			case '?':
				printf("Error: unknown option: -%c\n", optopt); break;
			default:
				if ((*V[opt%4])((optarg == NULL ? argv[0] : optarg)) == -1){
				}
		}
	}
	return 0;
}
