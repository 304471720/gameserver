#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "type.h"
#include "connection.h"
#include "log.h"

#define CONN_READ_TIMEOUT (300)
#define CONN_WRITE_TIMEOUT (300)

typedef struct _SERVER SERVER;

struct _WORKER {
	pthread_t thread_id;
	uint16_t ind;
	int sfd;
	struct event_base *base;
	CONN_LIST *conns;
	struct event notify_event;
	int notified_rfd;
	int notifed_wfd;
	LOG_QUEUE *qlog;
	SERVER *server;
};

typedef struct _WORKER_ARRAY {
	uint16_t size;
	SERVER *server;
	WORKER array[0];
} WORKER_ARRAY;

struct _SERVER {
	int server_fd;
	int port;
	struct event_base *base;
	struct event listen_event;
	WORKER_ARRAY *workers;
	LOG_QUEUE *qlog;
	int ret;
	uint16_t start_worker_num;
	uint64_t conn_count;
	pthread_mutex_t start_lock;//start_worker_num同步;cond wait队列同步
	pthread_cond_t start_cond;
};

SERVER* init_server(int port, uint16_t workernum, uint32_t connnum, int read_timeout, int write_timeout);
WORKER_ARRAY* init_workers(SERVER *server, uint16_t workernum, uint32_t connnum, int read_timeout, int write_timeout);
void* start_server(void* arg);
int start_workers(WORKER_ARRAY* workers);

#endif /* __NETWORK_H_ */
