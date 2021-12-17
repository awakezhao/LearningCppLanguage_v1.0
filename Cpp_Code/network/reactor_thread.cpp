
struct sockitem { //
	int sockfd; // 
	int (*callback)(int fd, int events, void *arg);

	char recvbuffer[1024]; //
	char sendbuffer[1024]; //


	// 

};

// mainloop / eventloop --> epoll -->  
struct reactor {

	int epfd;
	struct epoll_event events[512];
	

};

struct reactor *eventloop = NULL;


int recv_cb(int fd, int events, void *arg);


int send_cb(int fd, int events, void *arg) {

	struct sockitem *si = (struct sockitem*)arg;

	send(fd, "hello\n", 6, 0); //

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	//ev.data.fd = clientfd;
	si->sockfd = fd;
	si->callback = recv_cb;
	ev.data.ptr = si;

	epoll_ctl(eventloop->epfd, EPOLL_CTL_MOD, fd, &ev);

}

//  ./epoll 8080

int recv_cb(int fd, int events, void *arg) {

	//int clientfd = events[i].data.fd;
	struct sockitem *si = (struct sockitem*)arg;
	struct epoll_event ev;

	char buffer[1024] = {0};
	int ret = recv(fd, buffer, 1024, 0);
	if (ret < 0) {

		if (errno == EAGAIN || errno == EWOULDBLOCK) { //
			return -1;
		} else {
			
		}

		

		ev.events = EPOLLIN;
		//ev.data.fd = fd;
		epoll_ctl(eventloop->epfd, EPOLL_CTL_DEL, fd, &ev);

		close(fd);

		free(si);
		

	} else if (ret == 0) { //

		// 
		printf("disconnect %d\n", fd);

		

		ev.events = EPOLLIN;
		//ev.data.fd = fd;
		epoll_ctl(eventloop->epfd, EPOLL_CTL_DEL, fd, &ev);

		close(fd);

		free(si);
		
	} else {

		printf("Recv: %s, %d Bytes\n", buffer, ret);

		// fd --> full 
		// fd epollout 
		//ret = send(fd, buffer, ret, 0);
		//if (ret == -1) {

		// }
		// 
		// 

		struct epoll_event ev;
		ev.events = EPOLLOUT | EPOLLET;
		//ev.data.fd = clientfd;
		si->sockfd = fd;
		si->callback = send_cb;
		ev.data.ptr = si;

		epoll_ctl(eventloop->epfd, EPOLL_CTL_MOD, fd, &ev);

	}

}

int accept_cb(int fd, int events, void *arg) {

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(struct sockaddr_in));
	socklen_t client_len = sizeof(client_addr);
	
	int clientfd = accept(fd, (struct sockaddr*)&client_addr, &client_len);
	if (clientfd <= 0) return -1;

	char str[INET_ADDRSTRLEN] = {0};
	printf("recv from %s at port %d\n", inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
		ntohs(client_addr.sin_port));

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	//ev.data.fd = clientfd;

	struct sockitem *si = (struct sockitem*)malloc(sizeof(struct sockitem));
	si->sockfd = clientfd;
	si->callback = recv_cb;
	ev.data.ptr = si;
	
	epoll_ctl(eventloop->epfd, EPOLL_CTL_ADD, clientfd, &ev);
	
	return clientfd;
}

void *worker_thread(void *arg) {
    while (1)
    {
        int nready = epoll_wait(eventloop->epfd, eventloop->events, 512, -1);
		if (nready < -1) {
			break;
		}

        int i = 0;
		for (i = 0;i < nready;i ++) {

			if (eventloop->events[i].events & EPOLLIN) {
				//printf("sockitem\n");
				struct sockitem *si = (struct sockitem*)eventloop->events[i].data.ptr;
				si->callback(si->sockfd, eventloop->events[i].events, si);
			}

			if (eventloop->events[i].events & EPOLLOUT) {

				struct sockitem *si = (struct sockitem*)eventloop->events[i].data.ptr;
				si->callback(si->sockfd, eventloop->events[i].events, si);

			}
		}
    }
    
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
		return -1;
	}

	int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return -1;
	}

    struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
		return -2;
	}

	if (listen(sockfd, 5) < 0) {
		return -3;
	}

    eventloop = (struct reactor*)malloc(sizeof(struct reactor));
	// epoll opera

	eventloop->epfd = epoll_create(1);

    struct epoll_event ev;
	ev.events = EPOLLIN;

    struct sockitem *si = (struct sockitem*)malloc(sizeof(struct sockitem));
	si->sockfd = sockfd;
	si->callback = accept_cb;
	ev.data.ptr = si;

    epoll_ctl(eventloop->epfd, EPOLL_CTL_ADD, sockfd, &ev);

    pthread_t id;
	pthread_create(&id, NULL, worker_thread, NULL);

    while(1) {

	}
}