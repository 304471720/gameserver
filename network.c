#include "network.h"
#include "log.h"
#include "package_info.h"

void safe_close(int fd) {
	int ret = close(fd);
	while (ret != 0) {
		if (errno != EINTR || errno == EBADF)
			break;
		ret = close(fd);
	}
}

static int open_server_socket(const char *ip, short port, int backlog) {
	int fd = -1;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		fprintf(stderr, "cannot open server socket, errno: %d %m\n", errno);
		return -1;
	}

	unsigned long non_blocking = 1;
	if (ioctl(fd, FIONBIO, &non_blocking) != 0) {
		fprintf(stderr, "cannot set nonblocking, errno: %d %m\n", errno);
		safe_close(fd);
		return -1;
	}
	int flag_reuseaddr = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &flag_reuseaddr, sizeof(flag_reuseaddr));

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	memset(&addr, 0, addrlen);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip == NULL ? INADDR_ANY : inet_addr(ip);
	addr.sin_port = htons(port);

	if (bind(fd, (const struct sockaddr *) &addr, addrlen) != 0) {
		fprintf(stderr, "cannot bind, port: %d errno: %d %m\n", port, errno);
		safe_close(fd);
		return -1;
	}

	if (listen(fd, backlog) != 0) {
		fprintf(stderr, "cannot listen, port: %d errno: %d %m\n", port, errno);
		safe_close(fd);
		return -1;
	}

	return fd;
}

static int accept_client(int fd, struct sockaddr_in *s_in) {
	int cfd = -1;
	do {
		socklen_t len = sizeof(struct sockaddr_in);
		cfd = accept(fd, (struct sockaddr *) s_in, &len);
		if (cfd < 0) {
			if (errno == EINTR)
				continue;
			fprintf(stderr, "cannot accpet, errno: %d %m\n", errno);
			break;
		}
		unsigned long non_blocking = 1;
		if (ioctl(cfd, FIONBIO, &non_blocking) != 0) {
			fprintf(stderr, "cannot set nonblocking, client_fd: %d errno: %d %m\n", cfd, errno);
			safe_close(cfd);
			cfd = -1;
		}
		return cfd;
	} while (-1);
	return -1;
}

static void accept_action(int fd, short event, void *arg) {
	SERVER *s = (SERVER*) arg;
	struct sockaddr_in s_in;
	int cfd = accept_client(fd, &s_in);
	if (cfd == -1) {
		slog_err_t_w(s->qlog, "client accept error, errno: %d %m", errno);
		return;
	}

	int retry = 0;
	//轮流从工作线程池取一个线程
	//从工作线程池的conns队列取出一个conns
	//如果取出来是空的，再尝试找另外一个work线程
	//尝试次数最多worker size的一半
	do {
		WORKER* w = &s->workers->array[(s->conn_count++) % s->workers->size];
		CONN *citem;
		//johan: conns is a one-way queue; its out-way is owned by the server and its in-way is owned by one worker.
		GET_FREE_CONN(w->conns, citem);
		if (citem == NULL)
			continue;
		citem->fd = cfd;
		citem->cip = *(uint32_t *) &s_in.sin_addr;
		citem->cport = (uint16_t) s_in.sin_port;
		sprintf( citem->_id, "%s:%d", inet_ntoa(s_in.sin_addr ), ntohs(s_in.sin_port ) );
		fprintf(stdout,"%s  come in!!!\n",citem->_id);
		//notify worker to enable cfd's read and write
		write(w->notifed_wfd, (char *) &citem->ind, 4);

		if (AKG_FUNC[CMD_CONNECTED] != NULL)
			AKG_FUNC[CMD_CONNECTED](citem);
		return;
	} while (++retry < s->workers->size / 2);//half retry times is enough

	slog_err_t_w(s->qlog, "workers are too busy!");
	safe_close(cfd);
}

static void receive_notify_action(int fd, short event, void *arg) {
	WORKER *w = (WORKER*) arg;
	uint32_t ind;
	if (read(fd, &ind, 4) != 4) {
		slog_err_t_w(w->qlog, "notify_pipe read error, errno: %d %m", errno);
		return;
	}
	bufferevent_setfd(w->conns->list[ind].bufev, w->conns->list[ind].fd);
	bufferevent_enable(w->conns->list[ind].bufev, EV_READ | EV_WRITE);
}

void close_conn(CONN *c, uint16 command) {
	c->err_type = command;
	c->in_buf_len = 0;
	if (NULL != AKG_FUNC[command])
		AKG_FUNC[command](c);
	bufferevent_disable(c->bufev, EV_READ | EV_WRITE);
	safe_close(c->fd);
	PUT_FREE_CONN(c->owner->conns, c);
}

void close_err_conn(CONN *c) {
	close_conn(c, CMD_CLOSED);
}

static void on_conn_read(struct bufferevent *be, void *arg) {
	CONN *c = (CONN*) arg;
	while (1) {
		int fsize;
		while (1) {
			fsize = CONN_BUF_LEN - c->in_buf_len;
			if (0 == fsize)
				break;
			int len = bufferevent_read(be, c->in_buf + c->in_buf_len, fsize);
			if (len <= 0)
				break;
			c->in_buf_len += len;
		}
		while (1) {
			if (c->in_buf_len < SIZE_OF_HEAD) {
				if (0 == fsize)
					break;
				else
					return;
			}
			fflush(stdout);
			HEAD *h = (HEAD*) c->in_buf;
			HEAD req={0};
	       		req.package_len = ntohl(h->package_len);
        		req.command = ntohs(h->command);
			
			//if (req.package_len > MAX_PKG_LEN || req.package_len < SIZE_OF_HEAD) {
			if ((req.package_len + sizeof(req.package_len)) > MAX_PKG_LEN || (req.package_len+ sizeof(req.package_len)) < SIZE_OF_HEAD) {
				slog_err_t(c->owner->qlog, "pkg error, length: %u package_len: %u stx: 0x%x", c->in_buf_len, req.package_len,(uint32_t)0xffff0000);
				close_err_conn(c);
				return;
			}else if((req.package_len+ sizeof(req.package_len)) ==  SIZE_OF_HEAD)
			{
				uint8_t * tmp =  "pkg error, Only Command No data!!!\n";
				slog_err_t(c->owner->qlog,  "pkg error, Only Command No data!!!");
				//write(c->fd,tmp,strlen(tmp));
				return;
			}
			if (c->in_buf_len < req.package_len) {
				if (0 == fsize)
					break;
				else
					return;
			}

			if (req.command < CMD_CONNECTED  && AKG_FUNC[req.command]) {
				if (AKG_FUNC[req.command](c)) {
					return;
				}
			} else {
				slog_warn_t(c->owner->qlog, "invalid command: %u len %u", req.command, req.package_len);
				close_err_conn(c);
				return;
			}

			//c->in_buf_len -= req.package_len ;
			c->in_buf_len -= req.package_len+sizeof(req.package_len) ;
			//fprintf(stdout,"6-------%d-------c->in_buf_len \n",c->in_buf_len);
			//fflush(stdout);
			if (c->in_buf_len == 0) {
				if (0 == fsize)
					break;
				else
					return;
			}
			memmove(c->in_buf, c->in_buf + req.package_len,c->in_buf_len);
		}

	}
}

static void on_conn_err(struct bufferevent *be, short event, void *arg) {
	CONN *c = (CONN*) arg;
	uint16 id = -1;
	if (event & BEV_EVENT_TIMEOUT) {
		id = CMD_TIMEOUT;
	} else if (event & BEV_EVENT_EOF) {
		id = CMD_CLOSED;
	} else if (event & BEV_EVENT_ERROR) {
		id = CMD_CLOSED;
	}
	close_conn(c, id);
}

static void *start_worker(void *arg) {
	WORKER *w = (WORKER*) arg;
	SERVER *s = w->server;
	pthread_mutex_lock(&s->start_lock);
	++s->start_worker_num;
	pthread_cond_signal(&s->start_cond);
	pthread_mutex_unlock(&s->start_lock);
	event_base_loop(w->base, 0);
	fprintf(stderr, "start_worker error, thread_id: %lu\n", w->thread_id);
	return NULL;
}

SERVER* init_server(int port, uint16 workernum, uint32_t connnum, int read_timeout, int write_timeout) {
	g_authoritiesid = 0;
	LOG_QUEUE *lq = create_log_queue();
	if (lq == NULL)
		return NULL;
	fprintf(stderr, "--------------------------------------1\n");
	SERVER *s = (SERVER*) malloc(sizeof(SERVER));
	if (s == NULL) {
		fprintf(stderr, "init_server malloc error, errno: %d %m\n", errno);
		return NULL;
	}
	memset(s, 0, sizeof(SERVER));
	s->qlog = lq;
	s->port = port;
	s->base = event_init();
	if (s->base == NULL) {
		fprintf(stderr, "init_server event base error, errno: %m\n");
		return NULL;
	}
	fprintf(stderr, "--------------------------------------2\n");
	s->workers = init_workers(s, workernum, connnum, read_timeout, write_timeout);
	if (s->workers == NULL)
		return NULL;

	pthread_mutex_init(&s->start_lock, NULL);
	pthread_cond_init(&s->start_cond, NULL);
	fprintf(stderr, "--------------------------------------3\n");

	return s;
}

WORKER_ARRAY* init_workers(SERVER *server, uint16_t workernum, uint32_t connnum, int read_timeout,
		int write_timeout) {
	WORKER_ARRAY *workers = NULL;
	uint32_t len = sizeof(WORKER_ARRAY) + sizeof(WORKER) * workernum;
	workers = (WORKER_ARRAY*) malloc(len);
	if (workers == NULL) {
		fprintf(stderr, "init_workers malloc error, errno: %d %m\n", errno);
		return NULL;
	}
	memset(workers, 0, len);
	workers->size = workernum;
	workers->server = server;
	fprintf(stderr, "------------%p----------workernum%d ----------------!!!\n",server,workernum);
	int i=0;
	for (i = 0; i < workernum; i++) {
		int fds[2];
		if (pipe(fds)) {
			fprintf(stderr, "init_workers pipe error, errno: %d %m\n", errno);
			return NULL;
		}
		WORKER *w = &workers->array[i];
		LOG_QUEUE *lq = create_log_queue();
		if (lq == NULL) {
			fprintf(stderr, "init_workers qlog error, errno: %d %m\n", errno);
			return NULL;
		}
		w->qlog = lq;
		w->ind = i;
		w->notified_rfd = fds[0];
		w->notifed_wfd = fds[1];

		w->base = event_init();
		if (w->base == NULL) {
			fprintf(stderr, "init_workers event base error, errno: %d %m\n", errno);
			return NULL;
		}

		event_set(&w->notify_event, w->notified_rfd, EV_READ | EV_PERSIST, receive_notify_action, w);
		event_base_set(w->base, &w->notify_event);
		if (event_add(&w->notify_event, 0) == -1) {
			fprintf(stderr, "init_workers add event error, errno: %d %m\n", errno);
			return NULL;
		}

		CONN_LIST *lst = init_conn_list(connnum);
		if (lst == NULL) {
			fprintf(stderr, "init_workers conn_list error, errno: %d %m\n", errno);
			event_base_free(w->base);
			free(workers);
			return NULL;
		}
		//每个工作线程一个conns列表
		w->conns = lst;
		CONN *p = lst->head;
		while (p != NULL) {
			//为每个CONN绑定处理函数
			p->bufev = bufferevent_new(-1, on_conn_read, NULL, on_conn_err, p);
			bufferevent_base_set(w->base, p->bufev);
			bufferevent_settimeout(p->bufev, read_timeout, write_timeout);
			p->owner = w;
			p = p->next;
		}

		w->server = server;
		fprintf(stderr, "====================wokernum!!!%d\n",i);
	}
	return workers;
}

void* start_server(void *arg) {
	SERVER* server = (SERVER*) arg;
	server->ret = -1;
	if (start_workers(server->workers) == -1) {
		fprintf(stderr, "start workers error, errno: %m\n");
		return (void*) server->ret;
	}
	server->server_fd = open_server_socket(NULL, server->port, 1024);
	if (server->server_fd < 0) {
		fprintf(stderr, "open server socket error, errno: %d %m\n", errno);
		return (void*) server->ret;
	}
	event_set(&server->listen_event, server->server_fd, EV_READ | EV_PERSIST, accept_action, server);
	event_base_set(server->base, &server->listen_event);
	if (event_add(&server->listen_event, 0) == -1) {
		fprintf(stderr, "start server add listen event error, errno: %d %m\n", errno);
		return (void*) server->ret;
	}
	int i =0 ;
	for (i = 0; i < server->workers->size; i++)
		server->workers->array[i].sfd = server->server_fd;

	fprintf(stdout, "--------------start server ok--------------\n");
	fflush(stdout);
	event_base_loop(server->base, 0);
	fprintf(stderr, "start server loop error, errno: %d %m\n", errno);

	if(g_authoritiesid == 0)
		g_authoritiesid = DEFAULT_STX;
	server->ret = 0;
	return (void*) server->ret;
}

int start_workers(WORKER_ARRAY* workers) {
	int i =0;
	for (i = 0; i < workers->size; i++) {
		WORKER *w = &workers->array[i];
		if (pthread_create(&w->thread_id, NULL, start_worker, w) != 0) {
			fprintf(stderr, "start_workers create thread error, errno: %d %m\n", errno);
			return -1;
		}
	}
	SERVER *s = workers->server;
	while (s->start_worker_num < workers->size) {
		pthread_mutex_lock(&s->start_lock);
		pthread_cond_wait(&s->start_cond, &s->start_lock);
		pthread_mutex_unlock(&s->start_lock);
	}
	return 0;
}
