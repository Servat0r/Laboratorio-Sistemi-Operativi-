#include <stdio.h>
#include <errno.h>
#include <PMut.h>

void PMLock(pthread_mutex_t *mtx) {
	int err;
	if ( ( err=pthread_mutex_lock(mtx)) != 0 ) {
		errno=err;
		perror("lock");
		pthread_exit((void*)&errno); /* maybe */
	}
	//else printf("locked ");
}

void PMUnlock(pthread_mutex_t *mtx) {
	int err;
	if ( (err = pthread_mutex_unlock(mtx)) != 0) {
		errno = err;
		perror("lock");
		pthread_exit((void*)&errno);
	}
	//else printf("unlocked ");
}
