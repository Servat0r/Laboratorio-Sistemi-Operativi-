/* Server for Lab09.es2 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <defines.h>
#include "./util.h"
#include <assert.h>
#include <pthread.h>


typedef struct mtserver_s {

	struct sockaddr_un sa;
	int socket_fd;

} mtserver_t;

bool mtserver_init(mtserver_t* server, size_t n, const char* path){
	if ((!server) || (n < 0) || (!path) || (strlen(path) == 0) || (strlen(path) > UNIX_PATH_MAX-1)) {
		fprintf(stderr, "Error: correct initialization requires \
first argument a non-NULL mtserver_t object, second argument >= 0 and third \
argument a non-NULL and non-empty string whose length is < %d\n", UNIX_PATH_MAX - 1);
		return false;
	}
	server->sa.sun_family = AF_UNIX;
	strncpy(server->sa.sun_path, path, UNIX_PATH_MAX);
	server->socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (bind(server->socket_fd, (struct sockaddr*)&(server->sa), sizeof(server->sa)) == -1){
		perror("while assigning server name");
		return false;
	}
	if (n == 0) listen(server->socket_fd, 32);
	else listen(server->socket_fd, n);
	return true;
}

void* mtserver_work(void* data){
	int fd = *((int*)data); /* conn_fd */
	int res; /* read result */
	char buf[MAXBUFSIZE]; /* max characters read */
	char buf2[MAXBUFSIZE]; /* answer buffer */
	while (true){
		res = read(fd, buf, MAXBUFSIZE);
		/* Connection closed or 'EOS' sent */
		if ((res == 0) || (strlen(buf) == 0)) break;
		strncpy(buf2, buf, res);
		for (int i = 0; i < res; i++) buf2[i] = toupper(buf[i]);
		write(fd, buf2, res);
	}
	close(fd);
	free(data);
	return NULL;
}
#if 0
bool mtserver_run(mtserver_t* server, int maxconn){
	if (server == NULL) return false;
	if (maxconn < 0) return false;
	pthread_t tid; /* threadId of the least recent thread */
	int* cfd;
	int i = 0;
	while ((maxconn == 0) ^ (i < maxconn)){
		cfd = malloc(sizeof(int));
		*cfd = accept(server->socket_fd, NULL, 0);
		i++;
		printf("Connection #%d accepted\n", i);
		pthread_create(&tid, NULL, &mtserver_work, cfd);
		pthread_detach(tid);
		tid = 0;
		cfd = NULL;
	}
	return true;
}
#endif

#if 1
bool mtserver_run(mtserver_t* server){
	if (server == NULL) return false;
	pthread_t tid; /* threadId of the least recent thread */
	int* cfd;
	int i = 0;
	while (true){
		cfd = malloc(sizeof(int));
		*cfd = accept(server->socket_fd, NULL, 0);
		i++;
		printf("Connection #%d accepted\n", i);
		pthread_create(&tid, NULL, &mtserver_work, cfd);
		pthread_detach(tid);
		tid = 0;
		cfd = NULL;
	}
	return true;
}
#endif

bool mtserver_destroy(mtserver_t* server){
	if (server == NULL) return false;
	close(server->socket_fd);
	unlink(server->sa.sun_path);
	return true;
}

/* For standalone usage */
#if 1
#define USAGE "Usage: %s socketname\n"
int main(int argc, char* argv[]){
	if (argc < 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	mtserver_t server;
	assert(mtserver_init(&server, 0, argv[1]));
	assert(mtserver_run(&server));
	assert(mtserver_destroy(&server));
	return 0;
}
#endif
