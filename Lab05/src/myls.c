#define _POSIX_C_SOURCE  200112L  // needed for S_ISSOCK
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#define SEPARATOR "---------------------------------------\n"


void permessi(const char filename[]) {
    struct stat statbuf;
    /* int r;
    SYSCALL_EXIT(stat,r,stat(filename,&statbuf),"Facendo stat del nome %s: errno=%d\n",
	    filename, errno); */
    errno = 0;
    int r = stat(filename, &statbuf);
    if (r == -1) {
	fprintf(stderr, "Facendo stat del nome %s: errno = %d\n", filename, errno);
	exit(EXIT_FAILURE);
    }


    char mode[11] = {'-','-','-','-','-','-','-','-','-','-','\0'};
    if (S_ISDIR(statbuf.st_mode)) mode[0]='d';
    else if (S_ISCHR(statbuf.st_mode)) mode[0]='c';
    else if (S_ISBLK(statbuf.st_mode)) mode[0]='b';
    else if (S_ISLNK(statbuf.st_mode)) mode[0]='l';
    else if (S_ISFIFO(statbuf.st_mode)) mode[0]='p';
    else if (S_ISSOCK(statbuf.st_mode)) mode[0]='s';
    


    if (S_IRUSR & statbuf.st_mode) mode[1]='r';
    if (S_IWUSR & statbuf.st_mode) mode[2]='w';
    if (S_IXUSR & statbuf.st_mode) mode[3]='x';
    
    if (S_IRGRP & statbuf.st_mode) mode[4]='r';
    if (S_IWGRP & statbuf.st_mode) mode[5]='w';
    if (S_IXGRP & statbuf.st_mode) mode[6]='x';
    
    if (S_IROTH & statbuf.st_mode) mode[7]='r';
    if (S_IWOTH & statbuf.st_mode) mode[8]='w';
    if (S_IXOTH & statbuf.st_mode) mode[9]='x';
    
       
    struct passwd *pw = getpwuid(statbuf.st_uid);
    struct group  *gr = getgrgid(statbuf.st_gid);
    fprintf(stdout, "Name: %s\nI-node: %u\nSize: %ld bytes, %.4lf KB, %.4lf MB\nMode: %s\nUser: %s, Group: %s\nLast modified: %s", filename, (unsigned)statbuf.st_ino, statbuf.st_size, statbuf.st_size / 1024.0, statbuf.st_size / 1048576.0, mode, pw->pw_name, gr->gr_name, ctime(&statbuf.st_mtime));		
    fprintf(stdout, SEPARATOR);
}


int main(int argc, char * argv[])  {
    if (argc == 1) {
	fprintf(stderr, "usa: %s dir [dir]\n", argv[0]);
	return EXIT_FAILURE;
    }
    fprintf(stdout, SEPARATOR);
    for(int i=1;i<argc;++i) permessi(argv[i]);

    return 0;
}
