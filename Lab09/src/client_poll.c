/* Client for Lab09.es1 */

#include <stdio.h>
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
#include <util.h>
#include <assert.h>
#include <poll.h>

/* Timeout parameters (#conns without success) */
#define TIMEOUT_STRING "Server is taking too long to respond\n"
#define TIMEOUT_ATTEMPTS 5
/* Help string */
#define USAGE "Usage: %s socketname\n"


typedef struct client_s {
	int sock_fd;
	struct sockaddr_un sa;
	struct pollfd fds;
} client_t;

/** 
 * @brief Creates a new client.
*/
bool client_init(client_t* client){
	if (!client) return false;
	client->sa.sun_family = AF_UNIX;
	memset(client->sa.sun_path, '\0', UNIX_PATH_MAX);
	/* Initializes fd for stdin */
	client->sock_fd = -1;
	client->fds.fd = 0;
	client->fds.events = POLLOUT;
	client->fds.revents = 0;
	return true;
}


/**
 * @brief Mainloop for client: it reads a string from stdin and asks the server
 * to uppercase it, then it prints it to the stdout.
*/
//FIXME Error(s) handling!
bool client_work(client_t* client){
	if (client == NULL) return false;
	char buf[MAXBUFSIZE];	
	char buf2[MAXBUFSIZE];
	while (true){
		printf(">>> "); /* Prompt */
		fgets(buf, MAXBUFSIZE, stdin);
		if (isUseless(buf)) continue;
		//FIXME "quit   \n" does NOT work! (Even in Lab08.mybc)
		if (strncmp(buf, "quit\n", 5) == 0) break;		
		/* Checks whether the connection is still open */
		if (poll(&client->fds, 1, -1) == -1){
			perror("poll");
			return false;
		} else if ((client->fds.revents & POLLERR) || (client->fds.revents & POLLHUP)){
			printf("Server has closed connection\n");
			break;
		} else if (client->fds.revents & POLLOUT){
			/* First write, then read */
			write(client->sock_fd, buf, strlen(buf) + 1);
		} else {
			fprintf(stderr, "Connection error\n");
			return false;
		}
		/* Reads and checks whether the connection is still open */
		int res = read(client->sock_fd, buf2, MAXBUFSIZE);
		if (res == -1) { perror("read"); break; } /* TODO errno == ECONNRESET */
		if (res == 0) break; /* Server has closed connection */
		printf("Client got: %s\n", buf2);
		memset(buf, 0, MAXBUFSIZE);
		memset(buf2, 0, MAXBUFSIZE);
		client->fds.revents = 0;
	} /* while */
	return true;
}


/**
 * @brief Connects client to the server whose address is specified in sockpath,
 * storing this value in client->sa.
*/
bool client_run(client_t* client, char* sockpath){
	if ((client == NULL) || (sockpath == NULL)) return false;	
	strncpy(client->sa.sun_path, sockpath, UNIX_PATH_MAX);
	client->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0); /* Open socket */
	int conn;
	int i = 0;
	/* (Try to) connect */
	while (i < TIMEOUT_ATTEMPTS){
		conn = connect(client->sock_fd, (const struct sockaddr*)&(client->sa), sizeof(client->sa));
		if (conn == -1){
			if (errno == ENOENT){
				fprintf(stderr, "Connection #%d failed\n", i);
				sleep(1);
				i++;
			} else return false;
		} else break; /* Connection succeeded */
	}
	bool res;
	if (i == TIMEOUT_ATTEMPTS){
		printf("%s", TIMEOUT_STRING);
		res = false;
	} else {
		client->fds.fd = client->sock_fd;
		res = client_work(client);
	}
	close(client->sock_fd); /* Close connection and socket */
	client->sock_fd = -1;
	client->fds.fd = 0;
	memset(client->sa.sun_path, 0, UNIX_PATH_MAX);
	return res;
}


/**
 * @brief Destroys current client.
 */
//FIXME Maybe now this is useless
bool client_destroy(client_t* client){
	if (client == NULL) return false;
	client->sock_fd = 0;
	client->fds.fd = 0;
	client->fds.events = 0;
	client->fds.revents = 0;
	memset(client->sa.sun_path, 0, UNIX_PATH_MAX);
	return true;
}

int main(int argc, char* argv[]){
	if (argc < 2) {
		printf(USAGE, argv[0]);
		exit(EXIT_FAILURE);
	}
	client_t client;
	assert(client_init(&client));
	assert(client_run(&client, argv[1]));
	printf("Client disconnected\n");
	assert(client_destroy(&client));
	return 0;
}
