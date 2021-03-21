#include <stdio.h>
#include <stdlib.h>
#define N 2000
#define M 5

int n;
int m;

int main(int argc, char** argv){
	if (argc < 2){
		n = N;
	} else if (argc < 3){
		m = M;
	} else {
		n = atoi(argv[1]);
		m = atoi(argv[2]);
	}
	FILE* pippo = fopen("./pippo.txt","w");
	for (int i = 0; i < m; i++){
		for (int j = 0; j < n; j++){
			fprintf(pippo, "a");
		}
		fprintf(pippo, "\n");
	}
	fclose(pippo);
	return 0;
}
