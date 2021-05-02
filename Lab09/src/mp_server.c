/** 
 * Sequential server that handles multiple client connections using the select
 * system call.
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

typedef struct mpserver_s {
	int sock_fd; /* listening-socket file descriptor */
	fd_set cset; /* list of all active connection(s) file descriptor(s) */
	fd_set rdset; /* fd set of all 'ready-to-read' connection(s) */
	int maxfd; /* max fd for current active connection(s) */
	struct sockaddr_un sa; /* socket public address */
} mpserver_t;

/** @brief Initializes an mpserver_t object */
bool mpserver_init(mpserver_t* server, size_t n, const char* path){
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
	if (bind(server->sock_fd, (const struct sockaddr*)&(server->sa), sizeof(server->sa)) == -1){
		perror("While binding name to socket");
		return false;
	}
	listen(server->sock_fd, (n > 0 ? n : SOMAXCONN));
	FD_SET(server->sock_fd, &server->cset);
	server->maxfd = server->sock_fd; /* Ready to accept new connections */
	return true;
}

/** @brief Connection request handling */
bool mpserver_work(mpserver_t* server, int cfd, char buf[], char buf2[]){
	int res = read(cfd, buf, MAXBUFSIZE);
	/* Connection closed or 'EOS' sent */
	if ((res == 0) || (strlen(buf) == 0)) return false;
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
	bool res;
	while (true){
		server->rdset = server->cset;
		if (select(server->maxfd + 1, &server->rdset, NULL, NULL, NULL) == -1){
			perror("select");
			return false;
		} else {
			int maxfd = server->maxfd; /* could change during the iteration */
			for (fd = 0; fd <= maxfd; fd++){
				if (FD_ISSET(fd, &server->rdset)){
					if (fd == server->sock_fd){
						fd = accept(server->sock_fd, NULL, 0);
						FD_SET(fd, &server->cset);
						/* Updates maxfd after having added a new one */
						if (fd > server->maxfd) server->maxfd = fd;
						/* res == false <=> client has closed connection */
					} else if (!(res = mpserver_work(server, fd, buf, buf2))){			
						FD_CLR(fd, &server->cset);
						/* Updates maxfd after having removed one */
						server->maxfd = update(&server->cset, fd);
					}
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
	for (int i = 0; i < server->maxfd; i++){
		if (FD_ISSET(i, &server->cset)) close(i);
	}
	FD_ZERO(&server->cset);
	unlink(server->sa.sun_path);
	memset(&server->sa.sun_path, 0, UNIX_PATH_MAX);
	return true;
}

#if 1
#define USAGE "Usage: %s socketname\n"
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
#endif
