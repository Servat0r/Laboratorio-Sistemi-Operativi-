#if !defined(_PMUT_H)
#define _PMUT_H

#include <pthread.h>

void PMLock(pthread_mutex_t *mtx);

void PMUnlock(pthread_mutex_t *mtx);

#endif
