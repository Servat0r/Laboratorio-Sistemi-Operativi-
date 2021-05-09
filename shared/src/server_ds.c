#include <server_ds.h>

/** @brief Updates the maximum set fd of a fd_set after having removed one */
static int update(fd_set* set, int fd){
	while (fd >= 0){
		if (FD_ISSET(fd, set)) return fd;
		fd--;
	}
	return fd; /* == -1, i.e. no more fds available */
}

int cmanager_init(cmanager_t* cm, int socket){
	cm->sockfd = socket;
	FD_ZERO(&cm->cset);
	cm->maxfd = socket;
	FD_SET(socket, &cm->cset);
	SYSCALL_RETURN(rwlock_init(&cm->lock), -1, "rwlock_init");
	return 0;
}

int addConnection(cmanager_t* cm, int fd){
	rwlock_write_start(&cm->lock);
	FD_SET(fd, &cm->cset);
	if (fd > cm->maxfd){ cm->maxfd = fd; }
	rwlock_write_finish(&cm->lock);
	return 0;
}

int removeConnection(cmanager_t* cm, int fd){
	rwlock_write_start(&cm->lock);
	close(fd);
	FD_CLR(fd, &cm->cset);
	if (fd == cm->maxfd) { cm->maxfd = update(&cm->cset, fd); }
	rwlock_write_finish(&cm->lock);
	return 0;
}

int getSocketFD(cmanager_t* cm){ return cm->sockfd; }

int removeSocket(cmanager_t* cm){
	return removeConnection(cm, cm->sockfd);
}

int	removeAllConnections(cmanager_t* cm){
	rwlock_write_start(&cm->lock);
	int maxfd = cm->maxfd;
	for (int fd = 0; fd <= maxfd; fd++){
		if (FD_ISSET(fd, &cm->cset)){ close(fd); FD_CLR(fd, &cm->cset); }
	}
	rwlock_write_finish(&cm->lock);
	return 0;
}

int cmanager_destroy(cmanager_t* cm){
	removeAllConnections(cm);
	SYSCALL_RETURN(rwlock_destroy(&cm->lock), -1, "cmanager:rwlock_destroy");
	memset(cm, 0, sizeof(*cm));
	return 0;
}

/* ************************************************************************* */

int wmanager_init(wmanager_t* wm){
	wm->activeWorkers = 0;
	SYSCALL_RETURN(pthread_mutex_init(&wm->lock, NULL), -1, "wlock init");
	SYSCALL_RETURN(pthread_cond_init(&wm->wcond, NULL), -1, "wcond init");	
}

int	getActives(wmanager_t* wm){
	pthread_mutex_lock(&wm->lock);
	int ret = wm->activeWorkers;
	pthread_mutex_unlock(&wm->lock);
	return ret;
}

//FIXME Questa cosa può causare deadlock con le get/set di sopra
int	wmanager_wait(wmanager_t* wm){
	pthread_cond_wait(&wm->wcond, &wm->lock);
}
