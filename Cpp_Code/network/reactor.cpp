#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_LENGTH (1024)
#define MAX_EPOLL_EVENTS (1024 * 1024)
#define MAX_EPOLL_ITEM 102400
#define SERVER_PORT (8888)

#define LISTEN_PROT_COUNT 100

typedef int NCALLBACK(int, int, void*);

struct ntyevent {
    int fd;
    int events;
    void *arg;
    int (*callback)(int fd, int events, void *arg);

    int status;
    char buffer[BUFFER_LENGTH];
    int length;
    long last_active;
};

struct ntyreactor {
    int epfd;
    struct ntyevent *events; // 1024 * 1024
};

int recv_cb(int fd, int events, void *arg);
int send_cb(int fd, int events, void *arg);

// 设置fd对应的数据：fd，回调，以及其他参数
void nty_event_set(struct ntyevent *ev, int fd, NCALLBACK callback, void *arg) {

    ev->fd = fd;
    ev->callback = callback;
    ev->events = 0;
    ev->arg = arg;
    ev->last_active = time(NULL);

    return;
}

// 将etyevent对应的fd添加到epfd的红黑树中
int nty_event_add(int epfd, int events, struct ntyevent *ev) {

    struct epoll_event ep_ev = {0, {0}};
    ep_ev.data.ptr = ev;
    ep_ev.events = ev->events = events;

    int op;
    if (ev->status == 1) {
        op = EPOLL_CTL_MOD;
    } else {
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }

    if (epoll_ctl(epfd, op, ev->fd, &ep_ev) < 0) {
        printf("event add failed [fd=%d], events[%d]\n", ev->fd, events);
        return -1;
    }

    return 0;
}

// 删除红黑树的节点，空间没有释放，统一释放
int nty_event_del(int epfd, struct ntyevent *ev) {

    struct epoll_event ep_ev = {0, {0}};

    if (ev->status != 1) {
        return -1;
    }

    ep_ev.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ev->fd, &ep_ev);

    return 0;
}

int recv_cb(int fd, int events, void *arg) {

    struct ntyreactor *reactor = (struct ntyreactor*)arg;
    struct ntyevent *ev = reactor->events+fd;

    int len = recv(fd, ev->buffer, BUFFER_LENGTH, 0);
    nty_event_del(reactor->epfd, ev);

    if (len > 0) {

        ev->length = len;
        ev->buffer[len] = '\0';

        printf("C[%d]:%s\n", fd, ev->buffer);

        nty_event_set(ev, fd, send_cb, reactor);
        nty_event_add(reactor->epfd, EPOLLOUT, ev);

    } else if (len == 0) {
        close(ev->fd);
        printf("[fd=%d] pos[%d], closed\n", fd, ev-reactor->events)

    } else {

        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s\n", fd, errno, strerror(errno));
    }

    return len;
}

int send_cb(int fd, int events, void *arg) {

    struct ntyreacotr *reactor = (struct ntyreactor*)arg;
    struct ntyevent *ev = reactor->events+fd;

    int len = send(fd, ev->buffer, ev->length, 0);
    nty_event_del(reactor->fd, ev);
    if (len > 0) {

        printf("send[fd=%d], [%d]%s\n", fd, len, ev->buffer);

        nty_event_set(ev, fd, recv_cb, reactor);
        nty_event_add(reactor->epfd, EPOLLIN, ev);

    } else {

        close(ev->fd);

        nty_event_del(reactor->epfd, ev);
        printf("send[fd=%d] error %s\n", fd, strerror(errno));

    }

    return len;
}

int accept_cb(int fd, int events, void *arg) {

    struct ntyreactor *reactor = (struct ntyreactor*)arg;
    if (reactor == NULL) return -1;

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int clientfd = 0;

    if ((clientfd = accept(fd, (struct sockaddr*)&client_addr, &len)) == -1) {
        if (errno != EAGAIN && errno != EINTR) {

        }
        printf("accept: %s\n", strerror(errno));
        return -1;
    }

    int i = 0;
    do {

        int flag = 0;
        if ((flag = fcntl(clientfd, E_SETFL, O_NONBLOCK)) < 0) {
            printf("%s: fcntl nonblocking failed, %d\n", __func__, MAX_EPOLL_EVENTS);
            break;
        }

        nty_event_set(&reactor->events[clientfd], clientfd, recv_cb, reactor);
        nty_event_add(reactor->epfd, EPOLLIN, &reactor->events[clientfd]);

    } while(0);

    printf("new connect [%s:%d][time:%ld], pos[%d]\n",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), reactor->events[i].last_active, i);

    return 0;    
}

// 创建fd，在fd上绑定ip和端口
int init_sock(short port) {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (listen(fd, 20) < 0) {
        printf("listen failed : %s\n", strerror(errno));
    }

    printf("listen port : %d\n", port);

    return fd;
}

// 构造红黑树，分配events数组
int ntyreactor_init(struct ntyreactor *reactor) {

    if (reactor == NULL) return -1;
    memset(reactor, 0, sizeof(struct ntyreactor));

    reactor->epfd = epoll_create(1);
    if (reactor->epfd <= 0) {
        printf("Create epfd in %s err %s\n", __func__, strerror(errno));
        return -2;
    }

    reactor->events = (struct ntyevent*)malloc((MAX_EPOLL_EVENTS) * sizeof(struct ntyevent)); //todo
    if (reactor->events == NULL) {
        printf("create epfd in %s in err %s\n", __func__, strerror(errno));
        close(reactor->epfd);
        return -3;
    }
}

int ntyreactor_destroy(struct ntyreactor *reactor) {

    close(reactor->epfd);
    free(reactor->events);
}

int ntyreactor_addlistener(struct ntyreactor *reactor, int sockfd, NCALLBACK *acceptor) {

    if (reactor == NULL) return -1;
    if (reactor->events == NULL) return -1;

    //设置sockfd对应的数据，数据保存到reactor->events[sockfd]中
    nty_event_set(&reactor->events[sockfd], sockfd, acceptor, reactor);
    nty_event_add(reactor->epfd, EPOLLIN, &reactor->events[sockfd]);

    return 0;
}

int ntyreactor_run(struct ntyreactor *reactor) {

    if (reactor == NULL || reactor->epfd < 0 || reactor->events == NULL) return -1;

    struct epoll_event events[MAX_EPOLL_ITEM];

    int checkpos = 0, i;

    while (1)
    {
        
        int nready = epoll_wait(reactor->epfd, events, MAX_EPOLL_ITEM, 1000);
        if (nready < 0) {
            printf("epoll_wait error, exit\n");
            continue;
        }

        for (i = 0; i < nready; i++) {

            struct ntyevent *ev = (struct ntyevent*)events[i].data.ptr;

            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) {
                ev->callback(ev->fd, events[i].events, ev->arg);
            }
            
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT)) {
                ev->callback(ev->fd, events[i].events, ev->arg);
            }
        }
    }    

    return 0;
}

int main(int argc, char *argv[]) {

    unsigned short port = SERVER_PORT;
    if (argc == 2) {
        port = atoi(argv[i]);
    }
    struct ntyreactor *reactor = (struct ntyreactor*)malloc(sizeof(struct ntyreactor));
    ntyreactor_init(reactor); 
    
    int listenfd[LISTEN_PROT_COUNT] = {0};
    int i = 0;

    for (i = 0; i < LISTEN_PROT_COUNT; i++) {
        listenfd[i] = init_sock(port+i);
        ntyreactor_addlistener(reactor, listenfd[i], accept_cb);
    }
    
    ntyreactor_run(reactor);

    ntyreactor_destory(reactor);

    for (i = 0; i < LISTEN_PROT_COUNT; i++) {
        close(listenfd[i]);
    }

    return 0;
}