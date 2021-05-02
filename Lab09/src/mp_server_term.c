/** 
 * Sequential server that handles multiple client connections using the select
 * system call.
 * @Note: This is a variation on the exercise in which the server accepts
 * any input from stdin as a request to terminate, and closes all connections
 * with clients "on-demand", i.e. when a client makes another requests, it
 * closes connection. 
 * @author Salvatore Correnti
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <defines.h>
#include <util.h>
#include <assert.h>
#include <poll.h>

#define USAGE "Usage: %s socketname\n"

#define DBG

bool fd_iszero(fd_set* set, int fd_min, int fd_max){
	for (int i = fd_min; i <= fd_max; i++){
		if (FD_ISSET(i, set)) return false;
	}
	return true;
}

typedef struct mpserver_s {
	int sock_fd; /* listening-socket file descriptor */
	fd_set cset; /* list of all active connection(s) file descriptor(s) */
	fd_set rdset; /* fd set of all 'ready-to-read' connection(s) */
	int maxfd; /* max fd for current active connection(s) */
	struct sockaddr_un sa; /* socket public address */
	size_t actives; /* active (accepted) connections */
} mpserver_t;

/** @brief Initializes an mpserver_t object */
bool mpserver_init(mpserver_t* server, size_t n, const char* path){

	/* Params checking */
	if ((server == NULL) || (n < 0) || (path == NULL) || (strlen(path) == 0) || (strlen(path) >= UNIX_PATH_MAX)){
		fprintf(stderr, "Error: correct initialization requires \
first argument a non-NULL mtserver_t object, second argument >= 0 and third \
argument a non-NULL and non-empty string whose length is < %d\n", UNIX_PATH_MAX - 1);	
		return false;
	}

	server->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	server->sa.sun_family = AF_UNIX;
	strncpy(server->sa.sun_path, path, UNIX_PATH_MAX);

	FD_ZERO(&server->cset);
	FD_SET(0, &server->cset); /* Accepts input from stdin */

	SYSCALL_RETURN(bind(server->sock_fd, (const struct sockaddr*)&(server->sa), sizeof(server->sa)), false, "While binding name to socket");

	/* Sets server to listen */
	SYSCALL_RETURN(listen(server->sock_fd, (n > 0 ? n : SOMAXCONN)), false, "While setting socket to listen");

	FD_SET(server->sock_fd, &server->cset);
	server->maxfd = server->sock_fd; /* Ready to accept new connections */

	server->actives = 0; /* Ignores both stdin and server->sock_fd */

	return true;
}

/** @brief Connection request handling */
bool mpserver_work(mpserver_t* server, int cfd, char buf[], char buf2[]){
	struct pollfd wfd;
	wfd.fd = cfd;
	wfd.events = POLLOUT;
	int res = read(cfd, buf, MAXBUFSIZE);
	if (res <= 0) return false;
	strncpy(buf2, buf, res);
	for (int i = 0; i < res; i++) buf2[i] = toupper(buf[i]);
	if (poll(&wfd, 1, -1) == -1){ perror("poll"); return false; }
	if ((wfd.revents & POLLERR) || (wfd.revents & POLLHUP)) return false;
	write(cfd, buf2, res);
	return true;
}

/** @brief Updates the maximum set fd of a fd_set after having removed one */
static int update(fd_set* set, int fd){
	while (fd >= 0){
		if (FD_ISSET(fd, set)) return fd;
		fd--;
	}
	return fd; /* == -1, i.e. no more fds available */
}

static void closeAll(fd_set* set, int maxfd){
	for (int fd = 0; fd <= maxfd; fd++){
		if (FD_ISSET(fd, set)) close(fd);
	}
	FD_ZERO(set);
}

/** @brief Server mainloop */
bool mpserver_run(mpserver_t* server){

	if (server == NULL) return false;

	int fd; /* For new connections */
	char buf[MAXBUFSIZE];
	char buf2[MAXBUFSIZE];
	bool input = false; /* input == true <=> user has typed "...\n" on stdin */

	while (true){
#ifdef DBG
		printf("input = %d\nactives = %lu\nmaxfd = %d\n", (input ? 1 : 0), server->actives, server->maxfd);
		for (int i = 0; i <= server->maxfd; i++) printf("%d ", (FD_ISSET(i, &server->cset) ? 1 : 0));
		printf("\n");
#endif
		if (input && (server->actives == 0)){
			printf("Closing server...\n");
			break;
		}
		server->rdset = server->cset;
		if (select(server->maxfd + 1, &server->rdset, NULL, NULL, NULL) == -1){
			perror("select");
			closeAll(&server->cset, server->maxfd); /* Close all connections */
			printf("Closed all conns\n"); //FIXME Debug
			return false;
		} else {
			int maxfd = server->maxfd; /* could change during the iteration */
			if (!input && (FD_ISSET(0, &server->rdset))){
				input = true; /* User has typed '\n' */
				FD_CLR(0, &server->cset); /* No more input accepted */
				close(server->sock_fd);
				FD_CLR(server->sock_fd, &server->cset);
				FD_CLR(server->sock_fd, &server->rdset);
			}
			/* No input taken */
			if ((FD_ISSET(server->sock_fd, &server->rdset)) && !input){
				fd = accept(server->sock_fd, NULL, 0);
				printf("Accepted connection fd #%d\n", fd);
				FD_SET(fd, &server->cset);
				FD_CLR(server->sock_fd, &server->rdset); /* Already handled */
				/* Updates maxfd after having added a new one */
				if (fd > server->maxfd) server->maxfd = fd;
				server->actives++;
			}
			for (fd = 1; fd <= maxfd; fd++){
				if (FD_ISSET(fd, &server->rdset)){
					if (input){ /* Server closes connection */
						close(fd);
						FD_CLR(fd, &server->cset);
						server->actives--;
					} else {
						printf("Handling fd #%d\n", fd);
						if (!mpserver_work(server, fd, buf, buf2)) {
							printf("Closed connection\n");					
							/* Client has closed connection */
							close(fd);
							FD_CLR(fd, &server->cset);
							server->actives--;
						}
					}
					server->maxfd = update(&server->cset, maxfd);							

				} /* FD_ISSET */

			} /* for */

		} /* else (handle select) */

	} /* while */
	return true;
}

/** @brief Destroys current server */
bool mpserver_destroy(mpserver_t* server){
	if (server == NULL) return false;
	FD_CLR(server->sock_fd, &server->cset); /* Maybe not necessary */
	FD_ZERO(&server->rdset);
	for (int i = 0; i < server->maxfd; i++){
		if (FD_ISSET(i, &server->cset)) close(i);
	}
	FD_ZERO(&server->cset);
	unlink(server->sa.sun_path);
	memset(&server->sa.sun_path, 0, UNIX_PATH_MAX);
	return true;
}


int main(int argc, char* argv[]){
	if (argc < 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	mpserver_t server;
	assert(mpserver_init(&server, 0, argv[1]));
	assert(mpserver_run(&server));
	assert(mpserver_destroy(&server));
	return 0;
}
