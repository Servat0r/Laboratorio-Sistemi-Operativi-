#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Requires: in e out già allocati, len > 0 (INCLUSO '\0' alla fine)
void strtoupper(const char* in, size_t len, char* out){
	char* tempdest = strncpy(out, in, len);
	if (tempdest == NULL){
		perror("");
		return;
	}
	out = tempdest;
	tempdest = NULL;
	size_t inlen = strlen(in); //Lunghezza della stringa iniziale
	if (inlen >= len) out[len-1] = 0; //Terminatore
	for (size_t i = 0; i < len; i++){
		out[i] = toupper(out[i]);	
	}
	return;
	//Copia in in out e poi modifica lì

}

int main(size_t argc, char* argv[]){
	if (argc > 1){
		size_t lens;
		char* out;
		for (size_t i = 1; i < argc; i++){
			lens = strlen(argv[i]);
			out = (char*)malloc(lens);
			strtoupper(argv[i], lens, out);
			printf("%s\n", out);
		}
	}
	return 0;
}
