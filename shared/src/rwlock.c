/**
 * Example of read-write lock, which allows multiple readers but ONLY ONE writer
 * Reading methods (e.g. getX(), getY() etc.) should call a read_start(&rwl : rwlock)
 * BEFORE reading, and a read_finish(&rwl : rwlock) AFTER reading, and similar for
 * writing methods.
 */

#include <rwlock.h>
#include <util.h>


static pthread_mutex_t wsm = PTHREAD_MUTEX_INITIALIZER;

/**
 * @returns A pointer to a new rwlock_t object on success, NULL on error. 
*/
int rwlock_init(rwlock_t* rwl){
	if (rwl == NULL) return -1;
	rwl->activeReaders = 0;
	rwl->activeWriter = false;
	rwl->waitingReaders = 0;
	rwl->waitingWriters = 0;
	if (pthread_mutex_init(&rwl->lock, NULL) != 0) return -1;
	if (pthread_cond_init(&rwl->readers, NULL) != 0) return -1;
	if (pthread_cond_init(&rwl->writers, NULL) != 0) return -1;
	return 0;
}


/**
 * Checks whether current reader thread should wait.
 * @param rwl The rwlock to be checked for reading.
 * @returns true if the current reader thread should wait, false otherwise.
*/
static bool readerShouldWait(rwlock_t* rwl){
	return ((rwl->activeWriter) || (rwl->waitingWriters > 0));
}


/**
 * Checks whether current writer thread should wait.
 * @param rwl The rwlock to be checked for writing.
 * @returns true if the current writer thread should wait, false otherwise.
*/
static bool writerShouldWait(rwlock_t* rwl){
	return ((rwl->activeWriter) || (rwl->activeReaders > 0));
}


/**
 * Acquires rwlock object for starting reading.
 * @param rwl The rwlock to be acquired.
 * @returns 0 on success, -1 on error.
*/
int rwlock_read_start(rwlock_t* rwl){
	LOCK(&rwl->lock);
	rwl->waitingReaders++;
	while (readerShouldWait(rwl)) pthread_cond_wait(&(rwl->readers), &(rwl->lock));
	rwl->waitingReaders--;
	rwl->activeReaders++;
	UNLOCK(&rwl->lock);
	return 0;
}


/**
 * Releases rwlock object after reading.
 * @param rwl The rwlock to be released.
 * @returns 0 on success, -1 on error.
*/
int rwlock_read_finish(rwlock_t* rwl){
	LOCK(&rwl->lock);
	rwl->activeReaders--;
	if ((rwl->activeReaders == 0) && (rwl->waitingWriters > 0)){
		pthread_cond_signal(&rwl->writers);
	}
	UNLOCK(&rwl->lock);
	return 0;
}


/**
 * Acquires rwlock object for starting writing.
 * @param rwl The rwlock to be acquired.
 * @returns 0 on success, -1 on error.
*/
int rwlock_write_start(rwlock_t* rwl){
	LOCK(&rwl->lock);
	rwl->waitingWriters++;
	while (writerShouldWait(rwl)) pthread_cond_wait(&rwl->writers, &rwl->lock);
	rwl->waitingWriters--;
	rwl->activeWriter = true;
	UNLOCK(&rwl->lock);
	return 0;
}


/**
 * Releases rwlock object after writing.
 * @param rwl The rwlock to be released.
 * @returns 0 on success, -1 on error.
*/
int rwlock_write_finish(rwlock_t* rwl){
	LOCK(&rwl->lock);
	rwl->activeWriter = false;
	if (rwl->waitingWriters > 0){
		pthread_cond_signal(&rwl->writers);
	} else {
		pthread_cond_broadcast(&rwl->readers);
	}
	UNLOCK(&rwl->lock);
	return 0;
}


/**
 * Destroys rwlock object.
 * @param rwl The rwlock to be destroyed.
 * @returns 0 on success, -1 on error.
*/
//FIXME Notice that if rwl is heap-allocated, it is NOT freed by this procedure,
//because rwl could also have been declared on the stack
int rwlock_destroy(rwlock_t* rwl){
	int ret = pthread_mutex_destroy(&rwl->lock);
	if (ret != 0) return -1;
	ret = pthread_cond_destroy(&rwl->readers);
	if (ret != 0) return -1;
	ret = pthread_cond_destroy(&rwl->writers);
	if (ret != 0) return -1;
	return 0;
}

