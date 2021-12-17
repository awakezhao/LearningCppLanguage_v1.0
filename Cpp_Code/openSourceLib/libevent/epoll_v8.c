
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <errno.h>
#include <sys/epoll.h>


#define BUFFER_LENGTH 1024
#define LISTEN_PORT_COUNT 100
#define PORT (8080)

typedef int (*CallBack)(int fd, int events, void *arg);

struct sockitem { //

    int sockfd;
    CallBack callback;

    char recvbuffer[BUFFER_LENGTH]; //
	char sendbuffer[BUFFER_LENGTH];

    int rlength;
	int slength;
};

// reactor模型的数据
struct reactor {

	int epfd;
	struct epoll_event events[512];
};

struct reactor *eventloop = NULL;

int recv_cb(int fd, int events, void *arg);

int send_cb(int fd, int events, void *arg) {

    struct sockitem *si = (struct sockitem*)arg;

    send(fd, si->sendbuffer, si->slength, 0); //

    struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;

    si->sockfd = fd;
	si->callback = recv_cb;
	ev.data.ptr = si;

    epoll_ctl(eventloop->epfd, EPOLL_CTL_MOD, fd, &ev);

    return 0;

}

int recv_cb(int fd, int events, void *arg) {

    struct sockitem *si = (struct sockitem*)arg;
	struct epoll_event ev;

    int ret = recv(fd, si->recvbuffer, BUFFER_LENGTH, 0);
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) { //
			return -1;
		} else {
			
		}

        ev.events = EPOLLIN;
        epoll_ctl(eventloop->epfd, EPOLL_CTL_DEL, fd, &ev);

        close(fd);

		free(si);

    } else if (ret == 0) {

        printf("disconnect %d\n", fd);

        ev.events = EPOLLIN;

        epoll_ctl(eventloop->epfd, EPOLL_CTL_DEL, fd, &ev);

        close(fd);

		free(si);

    } else {

        printf("Recv: %s, %d Bytes\n", si->recvbuffer, ret);

        si->rlength = ret;
        memcpy(si->sendbuffer, si->recvbuffer, si->rlength);
        si->slength = si->rlength;

        struct epoll_event ev;
		ev.events = EPOLLOUT | EPOLLET;

        si->sockfd = fd;
		si->callback = send_cb;
		ev.data.ptr = si;

        epoll_ctl(eventloop->epfd, EPOLL_CTL_MOD, fd, &ev);
    }

    return 0;
    
}

struct sockitem* NewSockItem(int sockfd, CallBack cb);

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
    ev.data.ptr = (void *)NewSockItem(clientfd, recv_cb);

    epoll_ctl(eventloop->epfd, EPOLL_CTL_ADD, clientfd, &ev);
	
	return clientfd;
    
}

/*
    创建，绑定，监听
*/
int init_sock(short port) {

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

    printf ("listen port : %d\n", port);

	return sockfd;

}

struct sockitem* NewSockItem(int sockfd, CallBack cb) {

    struct sockitem *si = (struct sockitem*)malloc(sizeof(struct sockitem));
    si->sockfd = sockfd;
	si->callback = cb;
    return si;
}

int main(int argc, char *argv[]) {

    int port = 0;
    if (argc < 2) {
		port = PORT;
	} else {
        port = atoi(argv[1]);
    }

	eventloop = (struct reactor*)malloc(sizeof(struct reactor));

    eventloop->epfd = epoll_create(1);

    for (int i = 0; i < LISTEN_PORT_COUNT; i++) {

        int sockfd = init_sock(port+i);

        struct epoll_event ev;
		ev.events = EPOLLIN;
        
        ev.data.ptr = NewSockItem(sockfd, accept_cb);

        epoll_ctl(eventloop->epfd, EPOLL_CTL_ADD, sockfd, &ev);

    }

    for(;;) {

        int nready = epoll_wait(eventloop->epfd, eventloop->events, 512, -1);

        if (nready < -1) {
			break;
		}

        for (int i = 0; i < nready; i++) {

            if (eventloop->events[i].events & EPOLLIN) {

                struct sockitem *si = (struct sockitem*)eventloop->events[i].data.ptr;
                si->callback(si->sockfd, eventloop->events[i].events, si);
            }

            if (eventloop->events[i].events & EPOLLOUT) {

                struct sockitem *si = (struct sockitem*)eventloop->events[i].data.ptr;
                si->callback(si->sockfd, eventloop->events[i].events, si);
            }

        }
    }

    return 0;
}


