#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <myscandir.h>

#define MAXFILENAME 256
#define SEPARATOR "--------------------------------\n"

bool myfind(const char dirname[], const char filename[]){

	size_t len = strlen(filename);
	Queue* dq = initQueue();
	Queue* fq = initQueue();
	MyFileInfo* currentdir;
	MyFileInfo* currentfile;

	MyFileInfo* initial = malloc(sizeof(MyFileInfo));
	if (initial == NULL) return false;
	initial->pathname = getAbsolutePath(dirname);
	if (initial->pathname == NULL){
		fprintf(stderr, "Error: could not show absolute paths!\n");
		initial->pathname = dirname;
	}
	initial->filename = dirname;
	initial->size = 0;
	enqueue(dq, initial);

	int r;
	bool found = false;

	while (!isEmpty(*dq)){
		currentdir = dequeue(dq);
		r = myscandir(currentdir->pathname, dq, fq); 
		if (r != 0) return found;
		while (!isEmpty(*fq)){
			currentfile = dequeue(fq);
			if (strncmp(currentfile->filename, filename, len) == 0){
				found = true;
				printf("Path: %s\nLast modified: %s",currentfile->pathname, currentfile->date);
				printf(SEPARATOR);
			}
		}
	}
	return found;
} 



//Main completo !
int main(int argc, char* argv[]){
	if (argc != 3){
		printf("Usage: myfind <dirname> <filename>\n");
		return -1;
	}
	bool b = myfind(argv[1], argv[2]);
	if (b == true){
		return 0;
	} else {
		char* path = getAbsolutePath(argv[1]);
		fprintf(stderr, "Error: file %s not found in folder:\n\t%s\nand all its subfolders!\n", argv[2], path);
		return -1;
	}
}
