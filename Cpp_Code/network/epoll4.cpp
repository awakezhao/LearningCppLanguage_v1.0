#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <errno.h>
#include <sys/epoll.h>

struct sockitem {
	int sockfd;
	int (*callback)(int fd, int events, void *arg);

}

//  ./epoll 8080

int recv_cb(int fd, int events, void *arg) {

}


int accept_cb(int fd, int events, void *arg) {

	int clientfd = accept();

	struct sockitem *si = (struct sockitem*)malloc(sizeof(struct sockitem));
	si->sockfd = clientfd;
	si->callback = recv_cb;
	//
	epoll_ctl()
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

	// epoll opera

	int epfd = epoll_create(1);
	
	struct epoll_event ev, events[512] = {0};
	ev.events = EPOLLIN;
	ev.data.fd = sockfd; //int idx = 2000;
	

	struct sockitem *si = (struct sockitem*)malloc(sizeof(struct sockitem));
	si->sockfd = sockfd;
	si->callback = accept_cb;
	ev.data.ptr = si;
	
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	//pthread_cond_waittime();
	
	while (1) {
		int nready = epoll_wait(epfd, events, 512, -1);
		if (nready < -1) {
			break;
		}

		int i = 0;
		for (i = 0;i < nready;i ++) {	
			if (events[i].events & EPOLLIN) {
				struct sockitem *si = (struct sockitem*)events[i].data.ptr;
				si->callback(events[i].data.fd, events[i].events, si);
			}
			if (events[i].events & EPOLLOUT) {
                
			}
		}
	}
}