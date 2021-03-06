#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef enum {false, true} bool;

//Requires: gli argomenti variabili terminano SEMPRE con un 'NULL'
char* mystrcat(char* buffer, size_t buffer_size, char* prima, ...){
	buffer = strncat(buffer, prima, buffer_size);
	if (strlen(prima) >= buffer_size){
		buffer[buffer_size - 1] = 0;
		return buffer;
	}
	size_t current = strlen(prima);
	va_list ap;
	char* s;
	va_start(ap, prima);
	while (true) {
		s = va_arg(ap, char*);
		if (s == NULL) break; // !!!
		buffer = strncat(buffer, s, buffer_size - current);
		if (current + strlen(s) >= buffer_size){
			buffer[buffer_size - 1] = 0;
			return buffer;
		} else {
			current += strlen(s);
		}
	}
	return buffer;
}

/* Test da fare:
	[1] buffer_size <= strlen(prima); OK
	[2] ( NOT [1]) AND buffer_size <= Sum(strlen(...)); OK
	[3] (NOT [1]) AND buffer_size > Sum(strlen(...)); OK

int main(void){
	char prima[] = "Salvatore"; //strlen(prima) == 9, sizeof(prima) == 10
	char seconda[] = "Correnti"; //strlen(seconda) == 8, sizeof(seconda) == 9
	char terza[] = "Italia"; //strlen(terza) == 6, sizeof(terza) == 7
	size_t buffer_size = 64;
	char* buffer = (char*)malloc(buffer_size); //[1] con (<)
	buffer = mystrcat(buffer, buffer_size, prima, seconda, terza, NULL);
	printf("%s\n", buffer);
	return 0;
}
*/
