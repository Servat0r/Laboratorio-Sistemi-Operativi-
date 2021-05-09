/** 
 * @brief The same server as in Lab09.es3, but that is terminated by any of the
 * SIGINT/SIGQUIT/SIGTERM/SIGHUP signals and ignores SIGPIPE.
 *
 * @author Salvatore Correnti
 */

#define _POSIX_C_SOURCE 200809L

#include <defines.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <util.h>
#include <assert.h>
#include <signal.h>

#define SOCKNAME "bin/socket2"

#define USAGE "Usage: %s socketname\n"

typedef struct mpserver_s {
	volatile sig_atomic_t sock_fd; /* listening-socket file descriptor */
	fd_set cset; /* list of all active connection(s) file descriptor(s) */
	fd_set rdset; /* fd set of all 'ready-to-read' connection(s) */
	volatile sig_atomic_t maxfd; /* max fd for current active connection(s) */
	struct sockaddr_un sa; /* socket public address */
} mpserver_t;

/* Main server (declared as global variable in order to be caught by signal handler) */
mpserver_t mserver;

/* Flag that avoid multiple termination handlings */
volatile sig_atomic_t sig_caught = 0;


/** 
 * @brief Closes all opened connections (from 0 to server->maxfd included)
 * @return 0 on success, -1 on error.
 */
volatile sig_atomic_t closeAll(fd_set* set, volatile sig_atomic_t sockfd, volatile sig_atomic_t maxfd){
	volatile sig_atomic_t ret;
	if ((set == NULL) || (maxfd < 0)){ 
		ret = -1;
	} else {
		/* No more connections */
		close(sockfd);
		FD_CLR(sockfd, set);
		write(1, "\nClosing connections\n", 22);
		/* Close all connections */
		volatile sig_atomic_t fd;
		for (fd = 0; fd < maxfd; fd++){
			if (FD_ISSET(fd, set)) close(fd);
		}
		ret = 0;
	}
	return ret;
}

/** 
 * @brief Signal handler for terminating the server. Assumptions:
 *	1. When the handler is going on, NO other signals would be handled (i.e.,
 *	the signal mask would be reset after having destroyed the server);
 *	2. The server is the 'mserver' global variable declared above;
 *	3. The server address is defined in 'SOCKNAME';
 *	4. This termination is guaranteed only for SIGINT/SIGQUIT/SIGTERM/SIGHUP;
 *	5. This signal handler would destroy mserver, so calling mpserver_destroy()
 *		results in unspecified behaviour.
*/
void handle_termination(int sig){
	if (!sig_caught){
		sig_caught = 1;
		if ((sig == SIGINT) || (sig == SIGQUIT) || (sig == SIGTERM) || (sig == SIGHUP)){
			volatile sig_atomic_t ret = closeAll(&mserver.cset, mserver.sock_fd, mserver.maxfd);
			if (ret == -1){
				write(2, "Error when closing connections\n", 32);
				_exit(EXIT_FAILURE);
			}
			write(1, "Closing socket\n", 16);
			unlink(SOCKNAME);
		}
	}
}

/** @brief Initializes an mpserver_t object */
bool mpserver_init(mpserver_t* server, size_t n, const char* path){
	if ((server == NULL) || (n < 0) || (path == NULL) || (strlen(path) == 0) || (strlen(path) >= UNIX_PATH_MAX)){
		fprintf(stderr, "Error: correct initialization requires \
first argument a non-NULL mtserver_t object, second argument >= 0 and third \
argument a non-NULL and non-empty string whose length is < %d\n", UNIX_PATH_MAX - 1);	
		return false;
	}

	SYSCALL_EXIT((server->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)), "socket");
	server->sa.sun_family = AF_UNIX;
	strncpy(server->sa.sun_path, path, UNIX_PATH_MAX);

	FD_ZERO(&server->cset);

	SYSCALL_RETURN(bind(server->sock_fd, (const struct sockaddr*)&(server->sa), sizeof(server->sa)), \
		false, "While binding name to socket");

	SYSCALL_RETURN(listen(server->sock_fd, (n > 0 ? n : SOMAXCONN)), false, "While setting socket to listen");

	FD_SET(server->sock_fd, &server->cset);
	server->maxfd = server->sock_fd; /* Ready to accept new connections */

	return true;
}


/** @brief Connection request handling */
bool mpserver_work(mpserver_t* server, int cfd, char buf[], char buf2[]){
	int res = read(cfd, buf, MAXBUFSIZE);
	/* Connection closed */
	if (res <= 0) return false;
	strncpy(buf2, buf, res);
	for (int i = 0; i < res; i++) buf2[i] = toupper(buf[i]);
	write(cfd, buf2, res);
	return true;
}

/** @brief Updates the maximum set fd of a fd_set after having removed one */
static int update(fd_set* set, int fd){
	int i = fd-1; 
	while (i >= 0){
		if (FD_ISSET(i, set)) return i;
		i--;
	}
	return i; /* == -1, i.e. no more fds available */
}

/** @brief Server mainloop */
bool mpserver_run(mpserver_t* server){

	if (server == NULL) return false;

	int fd; /* For new connections */
	char buf[MAXBUFSIZE];
	char buf2[MAXBUFSIZE];

	while (true){
		server->rdset = server->cset;
		if (select(server->maxfd + 1, &server->rdset, NULL, NULL, NULL) == -1){
			switch(errno){
				case EINTR: break;
				default:
					perror("select");
			}
			return false;
		}
		//SYSCALL_RETURN(select(server->maxfd + 1, &server->rdset, NULL, NULL, NULL), false, "select");
		int maxfd = server->maxfd; /* could change during the iteration */
		for (fd = 0; fd <= maxfd; fd++){
			if (FD_ISSET(fd, &server->rdset)){
				if (fd == server->sock_fd){
					fd = accept(server->sock_fd, NULL, 0);
					FD_SET(fd, &server->cset);
					/* Updates maxfd after having added a new one */
					if (fd > server->maxfd) server->maxfd = fd;
					/* res == false <=> client has closed connection */
				} else if (!mpserver_work(server, fd, buf, buf2)){
					printf("Closed connection\n");
					close(fd);			
					FD_CLR(fd, &server->cset);
					/* Updates maxfd after having removed one */
					server->maxfd = update(&server->cset, fd);
				}
			}
		}
	}
	return true;
}

/** @brief Destroys current server */
bool mpserver_destroy(mpserver_t* server){
	if (server == NULL) return false;
	close(server->sock_fd);
	FD_CLR(server->sock_fd, &server->cset);
	FD_ZERO(&server->rdset);
	int i;
	for (i = 0; i < server->maxfd; i++){
		if (FD_ISSET(i, &server->cset)) close(i);
	}
	FD_ZERO(&server->cset);
	unlink(server->sa.sun_path);
	memset(&server->sa.sun_path, 0, UNIX_PATH_MAX);
	return true;
}


int main(void){

	/* Signal handlers registering */
	sigset_t set;
	sigset_t oldset;
	struct sigaction sa;
	struct sigaction sa_ign;
	SYSCALL_EXIT(sigfillset(&set), "sigfillset(1)");
	/* Signals masked */
	SYSCALL_EXIT(pthread_sigmask(SIG_SETMASK, &set, &oldset), "sigmask(1)");
	memset(&sa, 0, sizeof(sa)); /* Zeroes all */
	memset(&sa_ign, 0, sizeof(sa_ign));
	SYSCALL_EXIT(sigfillset(&sa.sa_mask), "sigfillset(2)"); /* Masks all signals when handling termination */
	sa.sa_handler = &handle_termination;
	sa_ign.sa_handler = SIG_IGN;
	SYSCALL_EXIT(sigaction(SIGINT, &sa, NULL), "sigaction[SIGINT]");
	SYSCALL_EXIT(sigaction(SIGQUIT, &sa, NULL), "sigaction[SIGQUIT]");
	SYSCALL_EXIT(sigaction(SIGTERM, &sa, NULL), "sigaction[SIGTERM]");
	SYSCALL_EXIT(sigaction(SIGHUP, &sa, NULL), "sigaction[SIGHUP]");
	SYSCALL_EXIT(sigaction(SIGPIPE, &sa_ign, NULL), "sigaction[SIGPIPE]");
	SYSCALL_EXIT(pthread_sigmask(SIG_SETMASK, &oldset, NULL), "sigmask(1)");
	/* Signals masked */
	/* Signal handlers registering */

	mpserver_init(&mserver, 0, SOCKNAME);
	mpserver_run(&mserver);
	mpserver_destroy(&mserver);

	return 0;
}
