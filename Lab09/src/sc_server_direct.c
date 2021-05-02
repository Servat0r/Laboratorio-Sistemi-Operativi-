/** @brief The same server as sc_server, but the subprocess 'bc' DIRECTLY
 * writes on connection-socket rather than on a pipe.
 * @author Salvatore Correnti
*/
#include <ctype.h>
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
#include <assert.h>


#define MAXTOTALCONN 3 /* Maximum number of accepted connections */
/* Error message when cannot launch bc */
#define ERRMSG "Error: cannot execute the requested operation\n"


typedef struct server1_s {
	int sock_fd; /* File descriptor of the listening socket */
	int conn_fd; /* File descriptor of the current connection-socket */
	struct sockaddr_un sa; /* struct for socket address */
} server1_t;

/**
 * @brief Creates a new server with name #SOCKNAME as defined in "defines1.h" if
 * none is already running, then make it listen for connections.
*/
//FIXME Error(s) handling!
bool server1_init(server1_t* server, size_t n, const char* path){
	if ((!server) || (n < 0) || (!path) || (strlen(path) == 0) || (strlen(path) >= UNIX_PATH_MAX)){
		fprintf(stderr, "Error: correct initialization requires \
first argument a non-NULL mtserver_t object, second argument >= 0 and third \
argument a non-NULL and non-empty string whose length is < %d\n", UNIX_PATH_MAX - 1);
		return false;
	}
	server->sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	server->sa.sun_family = AF_UNIX;
	strncpy(server->sa.sun_path, path, UNIX_PATH_MAX);
	if (bind(server->sock_fd, (struct sockaddr *)&(server->sa), sizeof(server->sa)) == -1){
		perror("While assigning a name to the socket");
		return false;
	}
	listen(server->sock_fd, (n > 0 ? n : SOMAXCONN));
	return true;
}


/**
 * @brief Handling requests: in this case, this starts a 'bc' subprocess for
 * calculating the result of the user input.
*/
//TODO This opens the possibility to ALWAYS write yourself into the socket in
//some way to avoid blocking when 'bc' does not write anything (ex. a = 9)
//FIXME Error(s) handling!
bool server1_work(server1_t* server){
	if (server == NULL) return false;
	char buf[MAXBUFSIZE];

	int pipe1[2]; /* Pipe file descriptor for executing 'bc' */
	int wstatus; /* wait status */

	while (true){
		int res = read(server->conn_fd, buf, MAXBUFSIZE);		
		if (res == 0) break; /* Client has closed connection */
		if (strlen(buf) == 0) break; /* EOS written */
		pipe(pipe1);
		pid_t pid = fork();
		switch(pid){
			case -1: {
				perror("fork");
				write(server->conn_fd, ERRMSG, strlen(ERRMSG));
				return false;
			}
			case 0: { /* Child, bc */
				close(server->sock_fd);
				close(pipe1[1]);
				dup2(pipe1[0], 0);
				dup2(server->conn_fd, 1);
				dup2(server->conn_fd, 2);
				close(server->conn_fd);
				close(pipe1[0]);
				execlp("bc", "bc", "-lq", (char*)NULL);
				perror("bc");
				read(0, buf, MAXBUFSIZE); /* Avoids SIGPIPE */
				exit(1);				
			}
			default: { /* Father */
				close(pipe1[0]);
				write(pipe1[1], buf, res);
				close(pipe1[1]); /* Equivalent to Ctrl-D for child */				
				wait(&wstatus);
#if 0
				//FIXME Client needs to read *<= 2* times!
				write(server->conn_fd, "\n", 2);
#endif
			}
		}
	}
	return true;
}


/**
 * @brief Mainloop for the current server.
*/
//FIXME Error(s) handling!
bool server1_run(server1_t* server){
	if (server == NULL) return false;
	bool ret = true;
	int i = 0;
	printf("Server is running.");
#if 0
	printf("Type anything on input to exit...\n");
	while (true){
#endif
#if 1
	printf("\n");
	while (i < MAXTOTALCONN){
		if (i >= MAXTOTALCONN) return true; /* No more connections */
#endif
		server->conn_fd = accept(server->sock_fd, NULL, 0);
		printf("Handling connection #%d\n", i+1);
		ret = ret && server1_work(server);
		close(server->conn_fd); /* Closes current connection */
		printf("Connection #%d ended\n", ++i);
	}
	printf("Total successful connections = %d\n", i);
	return true;
}


/**
* @brief Destroys current socket and unlinks socket temporary file(s).
*/
//FIXME Error(s) handling!
bool server1_destroy(server1_t* server){
	if (server == NULL) return false;
	close(server->sock_fd); /* No more connections */
	close(server->conn_fd); /* If there is any open connection */
	unlink(server->sa.sun_path); /* Unlinks temporary (socket) file */
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
	server1_t server;
	assert(server1_init(&server, 0, argv[1]));
	assert(server1_run(&server));
	assert(server1_destroy(&server));
	return 0;
}
#endif
