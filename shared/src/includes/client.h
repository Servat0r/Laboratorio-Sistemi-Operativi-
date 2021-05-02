#if !defined(_CLIENT_H)
#define _CLIENT_H

typedef struct client_s {
	int sock_fd;
	struct sockaddr_un sa;
} client_t;

bool
	client_init(client_t*),
	client_work(client_t*),
	client_run(client_t*, char*),
	client_destroy(client_t*);

#endif /* _CLIENT_H */
