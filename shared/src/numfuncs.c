#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

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
	return result;
}

bool isFPNumber(char* str){
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
	return result;
}

/** 
 * @brief Controls if the first argument corresponds to a (long) int and if so,
 * makes it available in the second argument.
 * @return  0 -> ok; 1 -> not a number; 2 -> overflow/underflow
 */
int getInt(const char* s, long* n) {
	errno=0;
	char* e = NULL;
	if (!isNumber(s)) return 1; /* not a number */
	long val = strtol(s, &e, 10);
	if (errno == ERANGE) return 2;    /* overflow/underflow */
	if (e != NULL && *e == (char)0) {
		*n = val;
		return 0;   /* success */
	}
	return 1;   /* not a number */
}



int getFloat(char* str, float* n){
	if (!isFPNumber(str)) return 1; /* Not a floating-point number */
	char* e = NULL;
	errno = 0;
	float val = (float)strtod(str, &e);
	if (errno == ERANGE) return 2; /* overflow/underflow */
	if (e != NULL && *e == (char)0){
		*n = val;
		return 0; /* success */
	}
	return 1; /* Not a floating-point number */
}
