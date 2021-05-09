/* Server for Lab10.es2 */

#define _POSIX_C_SOURCE 200809L

#include <defines.h>
#include <util.h>
#include <server_ds.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

#define USAGE "Usage: %s socketname\n"
#define SOCKNAME "bin/socket3"

#define SHUTDOWN_MSG "\nServer has received a signal SIGINT/SIGQUIT/SIGTERM, and so it will NOT \
accept ANY new request by clients aside from those currently processing\n"

#define CLOSE_MSG "\nServer has received a signal SIGTSTP/SIGHUP, ans so it will be available \
for all current clients but it does NOT accept new connection requests\n"


#define ERRSTR "Error: correct initialization requires \
first argument a non-NULL mtserver_t object, second argument >= 0 and third \
argument a non-NULL and non-empty string whose length is < %d\n"

/**
 *	SIGINT / SIGQUIT / SIGTERM / SIGTSTP / SIGHUP -> mainThread terminates server
 *	SIGPIPE -> ignored by handler
 *	others -> ignored by mainThread
*/

#define MAX(x,y) ((x) >= (y) ? (x) : (y));

#define DFREE(data) \
	free((int*)(data[1])); \
	free(data);

typedef struct mtserver_s {

	struct sockaddr_un sa; /* Server address */
	pthread_t dispatcher; /* threadId of the dispatcher thread */

	int pfd[2];
	cmanager_t cm;

	/* SHARED DATA */
	sstate_t state;
	pthread_mutex_t slock;
	/* SHARED DATA */

	/* SHARED DATA */
	pthread_mutex_t term_lock;
	int activeWorkers;
	pthread_cond_t term_cond; /* Main thread waits here */
	/* SHARED DATA */

} mtserver_t;

bool mtserver_destroy(mtserver_t*, sstate_t);

/* Debug option */
#if !defined(DEB)
void DEBUG(mtserver_t* server){
	static int count = 1;
	printf("Debug #%d\n", count++);
	printf("state = %d\n", server->state);
	printf("workers = %d\n", server->activeWorkers);
}
#else
void DEBUG(mtserver_t* server){ return; }
#endif


/* ************************************************************************* */
/** 
 * @brief Initializes server with the address specified in path and makes it
 * listen for new connections.
*/
bool mtserver_init(mtserver_t* server, size_t n, const char* path){
	if ((!server) || (!path) || (strlen(path) == 0) || (strlen(path) > UNIX_PATH_MAX-1)) {
		fprintf(stderr, ERRSTR, UNIX_PATH_MAX - 1);
		return false;
	}
	server->sa.sun_family = AF_UNIX;
	strncpy(server->sa.sun_path, path, UNIX_PATH_MAX);

	server->dispatcher = 0; /* Not already assigned */

	int sockfd;
	SYSCALL_EXIT((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)), "socket creation");
	SYSCALL_EXIT(pipe(server->pfd), "pipe");
	SYSCALL_EXIT(cmanager_init(&server->cm, sockfd), "cmanager init");

	server->state = S_OPEN;
	SYSCALL_EXIT(pthread_mutex_init(&server->slock, NULL), "slock init");

	SYSCALL_EXIT(pthread_mutex_init(&server->term_lock, NULL), "term_lock init");
	SYSCALL_EXIT(pthread_cond_init(&server->term_cond, NULL), "term_cond init");

	/* WRITE SHARED DATA */	
	LOCK(&server->term_lock);
	server->activeWorkers = 0;
	UNLOCK(&server->term_lock);
	/* WRITE SHARED DATA */

	SYSCALL_RETURN(bind(sockfd, (struct sockaddr*)&server->sa, sizeof(server->sa)), false, "while assigning server name");

	if (listen(sockfd, (n > 0 ? n : SOMAXCONN)) == -1){
		perror("While making server listen");
		mtserver_destroy(server, S_SHUTDOWN);
		return false;
	}

	printf("\nOn server init:\n");
	DEBUG(server);
	return true;
}

/* ************************************************************************* */
/** 
 * @brief Worker function.
*/
//FIXME Per il progetto sarebbe meglio che 'data' sia una entry di una hash table
//con tutte le info necessarie
void* mtserver_work(void** data){

	mtserver_t* server = (mtserver_t*)(data[0]);
	int fd = *(int*)(data[1]); /* conn_fd */

	pthread_mutex_lock(&server->term_lock);
	server->activeWorkers++;
	pthread_mutex_unlock(&server->term_lock);

	fd_set wset;
	fd_set wrdset;

	FD_ZERO(&wset);
	FD_SET(fd, &wset);
	FD_SET(server->pfd[0], &wset);

	int res; /* read result */
	char buf[MAXBUFSIZE]; /* max characters read */
	char buf2[MAXBUFSIZE]; /* answer buffer */
	int m = 1 + MAX(fd, server->pfd[0]);

	while (true){

		wrdset = wset;
		SYSCALL_EXIT(select(m, &wrdset, NULL, NULL, NULL), "Fatal error on select");
		if (FD_ISSET(server->pfd[0], &wrdset)){
			LOCK(&server->slock);
			while (server->state == S_OPEN) pthread_cond_wait(&server->term_cond, &server->slock);
			if (server->state == S_SHUTDOWN){
				UNLOCK(&server->slock);
				break;
			}
			UNLOCK(&server->slock);
		} else if (!FD_ISSET(fd, &wrdset)) break;
		printf("\nBefore read (%d)\n", fd);
		DEBUG(server);

		/* READ */
		res = read(fd, buf, MAXBUFSIZE);
		if (res == -1){ /* An error occurred */
			perror("read");
			break;
		} else if (res == 0){ /* Client has closed connection */
			printf("Connection #%d closed by client\n", fd);
			break;
		}
		/* READ */

		strncpy(buf2, buf, res);
		for (int i = 0; i < res; i++) buf2[i] = toupper(buf[i]);

		printf("\nBefore write (%d)\n", fd);
		DEBUG(server);

		/* WRITE */
		/* The sense is that when a request has been read, it ALWAYS completes */ 
		if (write(fd, buf2, res) == -1){
			if (errno == EPIPE){ /* Client has closed connection */
				printf("Connection #%d closed by client\n", fd);
				break;
			} else if (errno == EBADF){ /* Connection closed by server */
				DFREE(data);
				return NULL;
			} else { /* Other errors */
				int errno_copy = errno;
				fprintf(stderr, "When writing on connection #%d", fd);
				errno = errno_copy;
				perror("");
				break;
			}
		}
		/* WRITE */
		printf("\nAfter write (%d)\n", fd);
		DEBUG(server);
	}

	removeConnection(&server->cm, fd);

	printf("\nAfter work-mainloop (%d)\n", fd);	
	DEBUG(server);
	DFREE(data);

	pthread_mutex_lock(&server->term_lock);
	server->activeWorkers--;
	printf("\nBefore exiting (%d)\n", fd);
	DEBUG(server);
	if (server->state != S_OPEN && server->activeWorkers == 0) pthread_cond_broadcast(&server->term_cond);
	pthread_mutex_unlock(&server->term_lock);


	return NULL;
}

/* ************************************************************************* */
/** 
 * @brief Handles new connections.
*/
void* mtserver_dispatch(mtserver_t* server){

	pthread_t tid; /* threadId of the least recent thread */
	void** data;
	int nfd;
	int sockfd = getSocketFD(&server->cm);
	fd_set dset;
	fd_set rdset;
	FD_ZERO(&dset);
	FD_SET(sockfd, &dset);
	FD_SET(server->pfd[0], &dset);
	int m = 1 + MAX(sockfd, server->pfd[0]);
	while (true){

		rdset = dset;

		printf("\nBefore dispatching\n");
		DEBUG(server);
		
		SYSCALL_EXIT(select(m, &rdset, NULL, NULL, NULL), "select (dispatcher)");
		if (FD_ISSET(server->pfd[0], &rdset)){
			LOCK(&server->slock);
			while (server->state == S_OPEN) pthread_cond_wait(&server->term_cond, &server->slock);			
			UNLOCK(&server->slock);
			break;
		} else if (FD_ISSET(sockfd, &rdset)){

			nfd = accept(sockfd, NULL, 0);
			if (nfd == -1) break;

			data = calloc(2, sizeof(void*));
			data[0] = server;
			data[1] = malloc(sizeof(int)); 
			*((int*)data[1]) = nfd;

			printf("Connection accepted on file #%d\n", nfd);
			pthread_create(&tid, NULL, &mtserver_work, (void*)data);
			pthread_detach(tid);
			addConnection(&server->cm, nfd);			
		} else break;

		tid = 0;
		data = NULL;
		nfd = -1;
		printf("\nAfter dispatching\n");
		DEBUG(server);

	}
	removeSocket(&server->cm);
	printf("\nExiting (dispatcher)\n");
	return NULL;
}

/* ************************************************************************* */
/** 
 * @brief Creates dispatchThread and returns to main.
*/
bool mtserver_run(mtserver_t* server){
	if ((server == NULL) || (server->dispatcher != 0)) return false;
	pthread_create(&server->dispatcher, NULL, &mtserver_dispatch, (void*)server);
	return true;
}

/* ************************************************************************* */
/** 
 * @brief Destroys the current server.
*/
/* Executed for SIGINT/... handling in main */
bool mtserver_destroy(mtserver_t* server, sstate_t mode){
	if (server == NULL) return false;
	if (mode == S_OPEN) return false;

	close(server->pfd[1]); /* Unblocks select. */

	LOCK(&server->slock);
	server->state = mode; //FIXME Modify later with also S_CLOSE
	pthread_cond_broadcast(&server->term_cond);
	UNLOCK(&server->slock);
	
	//FIXME Qui c'era l'if-else sullo stato	

	printf("%s", (mode == S_SHUTDOWN ? SHUTDOWN_MSG : CLOSE_MSG));
	printf("Before destroying\n");
	DEBUG(server);
	

	pthread_mutex_lock(&server->term_lock);
	while (server->activeWorkers > 0) pthread_cond_wait(&server->term_cond, &server->term_lock);
	pthread_mutex_unlock(&server->term_lock);

	removeAllConnections(&server->cm);

	//if (server->state == S_SHUTDOWN) removeAllConnections(&server->cm); /* Closes IMMEDIATELY ALL connections */
	//else removeSocket(&server->cm); /* Closes SOCKET ONLY */

	/* Only main and dispatcher are still alive */

	printf("Joining dispatcher\n");

	pthread_join(server->dispatcher, NULL);

	printf("Closing pfd[0]\n");
	close(server->pfd[0]);

	unlink(server->sa.sun_path);

	/* Only main is still alive */

	printf("Destroying cmanager\n");
	cmanager_destroy(&server->cm);

	printf("Destroying slock\n");
	pthread_mutex_destroy(&server->slock);	

	printf("Destroying term_*\n");
	pthread_mutex_destroy(&server->term_lock);
	pthread_cond_destroy(&server->term_cond);

	memset(server, 0, sizeof(*server));

	printf("\nAfter destroying\n");
	DEBUG(server);
	return true;
}

/* ************************************************************************* */

int main(void){
	sigset_t set, oldset;
	sigset_t wset; /* sigwait set */
	int sig;
	struct sigaction sa_ign;
	bool termsig_caught = false; /* true iff SIGINT/.../SIGHUP has been caught */
	SYSCALL_EXIT(sigfillset(&set), "sigfillset(1)");

	/* Signals masked*/
	SYSCALL_EXIT(pthread_sigmask(SIG_SETMASK, &set, &oldset), "sigmask(1)");

	memset(&sa_ign, 0, sizeof(sa_ign));
	sa_ign.sa_handler = SIG_IGN;
	SYSCALL_EXIT(sigaction(SIGPIPE, &sa_ign, NULL), "sigaction[SIGPIPE]");

	mtserver_t server;
	mtserver_init(&server, 0, SOCKNAME);
	mtserver_run(&server);

	//FIXME There is already an executing server (need unlink and destroy)
	if (pthread_sigmask(SIG_SETMASK, &oldset, NULL) == -1){
		perror("sigmask(2)");
		mtserver_destroy(&server, S_SHUTDOWN);
	}
	/* Signals masked */

	/* Mainloop */
	//FIXME In questo caso funziona, ma in generale NO, perché se arrivano più
	//segnali contemporaneamente, bisognerebbe gestirli fino a svuotare la maschera.
	//FIXME E se arrivano due segnali di fila??
	while (true){
		sigfillset(&wset);
		pthread_sigmask(SIG_SETMASK, &wset, NULL);
		sigwait(&wset, &sig); /* ALL signals (?) */
		if (((sig == SIGINT) || (sig == SIGQUIT) || (sig == SIGTERM)) && !termsig_caught){
			termsig_caught = true;
			mtserver_destroy(&server, S_SHUTDOWN);
		} else if (((sig == SIGTSTP) || (sig == SIGHUP)) && !termsig_caught){
			termsig_caught = true;
			mtserver_destroy(&server, S_CLOSE);
		} else continue; /* Ignores other signals */
		printf("Server destroyed\n");
		break;
	}

	return 0;
}
