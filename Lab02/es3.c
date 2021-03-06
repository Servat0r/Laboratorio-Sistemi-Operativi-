#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
	if (argc != 3){
		fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
		return -1;
	}

	char* saveptr1 = argv[1];
	char* saveptr2 = argv[2];

	char* token1 = strtok_r(argv[1], " ", &saveptr1);

	size_t len = strlen(argv[2]);
	char* argcpy = malloc(len + 1);
	strcpy(argcpy, argv[2]);
	
	while(token1){
		strcpy(argv[2],argcpy);
		printf("%s\n", token1);
		char* token2 = strtok_r(argv[2], " ", &saveptr2);
		while(token2){
			printf("%s\n", token2);
			token2 = strtok_r(NULL, " ", &saveptr2);		
		}
		token1 = strtok_r(NULL, " ", &saveptr1);
	}
	free(argcpy);
	return 0;
}
