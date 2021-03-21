#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {false, true} bool;

//Checks if 'str' contains a "correct" long int
bool isNumber(char* str, long* val){
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
		long v = strtol(str, &e, 10);
		if (errno == ERANGE){ result = false; } //Overflow
		if (result) {*val = v;}
	}
	return result;
}

//Checks if 'str' contains a "correct" float number
bool isFPNumber(char* str, float* val){
	if (str == NULL) return false; //MUST check !
	if (strlen(str) == 0) return false; //MUST check ! ##missing
	bool result = false;
	int current = 0;
	bool foundDot = false; 
	int length = strlen(str); // > 0 a questo punto
	if (str[0] == '-') current = 1;
	//Questo controlla già che sia (almeno in teoria) un numero
	while (current < length){
		if (str[current] >= '0' && str[current] <= '9'){
			result = true;
			current++;
		} else if ((str[current] == '.') && (!foundDot)){
			foundDot = true;
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
		float v = (float)strtod(str, &e);
		if (errno == ERANGE){ result = false; } //Overflow
		if (result) {*val = v;}
	}
	return result;
}
