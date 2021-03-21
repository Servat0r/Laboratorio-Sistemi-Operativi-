#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <myscandir.h>


#if !defined(MAXFILENAME)
#define MAXFILENAME 2048
#endif


/*
Queue *dq => for saving ALL found directories different from '.' and '..'
Queue *fq => for saving ALL files found in '.' 
*/
int myscandir(const char nomedir[], Queue* dq, Queue* fq) {
    // controllo che il parametro sia una directory
    struct stat statbuf;
    int r;
   
    if ((r=stat(nomedir,&statbuf)) == -1) {
	perror("stat");
	return -1;
    }
    if(S_ISDIR(statbuf.st_mode)) {
	DIR * dir;
	if ((dir=opendir(nomedir)) == NULL) {
	    perror("opendir");
	    return -1;
	} else {
	    struct dirent *file;    
	    while((errno=0, file = readdir(dir)) != NULL) {
		struct stat statbuf2;
		char filename[MAXFILENAME]; 
		int len1 = strlen(nomedir);
		int len2 = strlen(file->d_name);
		if ((len2 == 1)  && (file->d_name[0] == '.')){
			continue;
		} else if ((len2 == 2) && (file->d_name[0] == '.') && (file->d_name[1] == '.')){
			continue;
		}
		if ((len1+len2+2)>MAXFILENAME) {
		    fprintf(stderr, "ERRORE: MAXFILENAME troppo piccolo\n");
		    exit(EXIT_FAILURE);
		}	    
		strncpy(filename,nomedir,      MAXFILENAME-1);
		strncat(filename,"/",          MAXFILENAME-1);
		strncat(filename,file->d_name, MAXFILENAME-1);
	    
		if (stat(filename, &statbuf2)==-1) {
		    perror("eseguendo la stat");
		    return -1;
		}
		char mode[11] = {'-','-','-','-','-','-','-','-','-','-','\0'};	
		if(S_ISDIR(statbuf2.st_mode)) {
		    mode[0]='d';
		}
		if (S_IRUSR & statbuf2.st_mode) mode[1]='r';
		if (S_IWUSR & statbuf2.st_mode) mode[2]='w';
		if (S_IXUSR & statbuf2.st_mode) mode[3]='x';
		
		if (S_IRGRP & statbuf2.st_mode) mode[4]='r';
		if (S_IWGRP & statbuf2.st_mode) mode[5]='w';
		if (S_IXGRP & statbuf2.st_mode) mode[6]='x';
		
		if (S_IROTH & statbuf2.st_mode) mode[7]='r';
		if (S_IWOTH & statbuf2.st_mode) mode[8]='w';
		if (S_IXOTH & statbuf2.st_mode) mode[9]='x';

		//Copiamo le informazioni nel file_info per portarle sopra
		MyFileInfo* file_info = malloc(sizeof(MyFileInfo));
		if ((file_info->pathname = malloc(len1+len2+2)) == NULL){
			perror("durante la scansione");
			return -1; //Facciamo galleggiare l'errore
		}	
		if (strncpy(file_info->pathname, filename, len1+len2+2) == NULL) perror("");

		if ((file_info->filename = malloc(MAXFILENAME)) == NULL){
			perror("durante la scansione");
			return -1;
		}
		if (strncpy(file_info->filename, file->d_name, MAXFILENAME) == NULL) perror("");

		if ((file_info->mode = malloc(11)) == NULL){ 
			perror("durante la scansione");
			return -1;
		}
		if (strncpy(file_info->mode, mode, 11) == NULL) perror("");

		file_info->size = statbuf2.st_size;

		if ((file_info->date = malloc(64)) == NULL){ 
			perror("durante la scansione");
			return -1;
		}
		if (strncpy(file_info->date, ctime(&statbuf2.st_mtime), 64) == NULL) perror("");
		
		//Aggiungiamo alla coda di directories / files
		if ((mode[0] == 'd') && (enqueue(dq, file_info))){
			continue;
		} else if ((mode[0] == '-') && (enqueue(fq, file_info))){
			continue;
		} else {
			fprintf(stderr, "Error when working with %s\n", filename);
			return -1;
		} 
	    }
	}
	if (errno != 0) perror("readdir");
	closedir(dir);	
    } else 
	fprintf(stderr, "%s non e' una directory\n", nomedir);
    return 0;    	      
}

//Stampa il contenuto di una directory (files e directories separatamente)
int printdir(const char pathname[]){
	Queue* dq = initQueue();
	Queue* fq = initQueue();
	int r = myscandir(pathname, dq, fq);
	if (r != 0) return r;
	MyFileInfo* file;
	printf("Directories:\n\n");
	while (!isEmpty(*dq)){
		file = dequeue(dq);
		printf("%s\t%lu\t%s\n",file->filename, file->size, file->mode);
		free(file);	
	}
	printf("------------------------\nFiles:\n\n");
	while (!isEmpty(*fq)){
		file = dequeue(fq);
		printf("%s\t%lu\t%s\n",file->filename, file->size, file->mode);
		free(file);
	}
	return 0;
}
