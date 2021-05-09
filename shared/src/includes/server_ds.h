/**
 * Internal data structures of the multithreaded server, in particular:
 * - an hashTable+fd_set for maintaining current client-connections state;
 * - a struct for tracking current active/waiting/closed workers;
 * - an enum for the possible states of the server
*/

#if !defined(_SERVER_DS_H)
#define _SERVER_DS_H


#include <defines.h>
#include <rwlock.h>
#include <sys/select.h>

/**
 * @brief Possible states of the server.
 * S_OPEN -> The server is open for new connections;
 * S_CLOSE -> The server is closed for new connections, but it will remain online
 *	until all existing connections will be closed;
 * S_SHUTDOWN -> The server is closed for new connections and will not accept ANY
 *	NEW request from the clients (but will serve all that are already arrived).
*/
typedef enum {S_OPEN, S_CLOSE, S_SHUTDOWN} sstate_t;

/* ************************************************************************* */

typedef struct cmanager_s {
	int sockfd;
	fd_set cset;
	int maxfd;
	rwlock_t lock;
} cmanager_t;

int cmanager_init(cmanager_t*, int),
	addConnection(cmanager_t*, int),
	removeConnection(cmanager_t*, int),
	getSocketFD(cmanager_t*),
	removeSocket(cmanager_t*),
	removeAllConnections(cmanager_t*),
	cmanager_destroy(cmanager_t*);

/* ************************************************************************* */

typedef struct wmanager_s {
	int activeWorkers;
	pthread_mutex_t lock;
	pthread_cond_t wcond;
} wmanager_t;

int wmanager_init(wmanager_t*),
	getActives(wmanager_t*),
	wmanager_wait(wmanager_t*);

#endif /* _SERVER_DS_H */
