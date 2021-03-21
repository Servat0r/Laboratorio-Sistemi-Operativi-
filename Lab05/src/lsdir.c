#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <myscandir.h>
#define SEPARATOR "--------------------------------\n"

int lsdir(const char pathname[]){

	Queue* dq = initQueue();
	Queue* fq = initQueue();
	int r = 0;
	MyFileInfo* current;
	MyFileInfo* initial = malloc(sizeof(MyFileInfo));
	initial->pathname = pathname;
	initial->size = 0;
	initial->mode = NULL;
	initial->filename = NULL;
	enqueue(dq, initial);

	while(!isEmpty(*dq)){
		current = dequeue(dq);
		printf("Directory: %s\n", current->pathname);
		r = myscandir(current->pathname, dq, fq);
		if (r != 0) return r;
		free(current);
		while (!isEmpty(*fq)){
			current = dequeue(fq);
			printf("%s\t%lu\t%s\n", current->filename, current->size, current->mode);
			free(current);		
		}
		printf(SEPARATOR); 
	} 
	return 0;
}


int main(int argc, char* argv[]){
	if (argc != 2){
		printf("Usage: lsdir <dir_name>\n");
		return -1;
	}
	return lsdir(argv[1]);
}
