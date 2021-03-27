#if !defined(_MYSCANDIR_H)
#define _MYSCANDIR_H

#include <myqueue.h> 

typedef struct myfileinfo {
	char* pathname;
	char* filename;
	size_t size;
	char* mode;
	char* date; //Last modified date
} MyFileInfo;

int myscandir(const char nomedir[], Queue* dq, Queue* fq);

int printdir(const char pathname[]);

char* getAbsolutePath(const char pathname[]);

#endif
