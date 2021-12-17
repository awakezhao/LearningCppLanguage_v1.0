


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

    int epfd = epoll_create(1);

    struct epoll_event ev, events[512] = {0};
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    
    while (1)
    {
        int nready = epoll_wait(epfd, events, 512, -1);
        if (nready < -1) {
            break;
        }

        int i = 0;
        for (i = 0; i < nready; i++) {
            if (events[i].events & EPOLLIN) {

                if (events[i].data.fd == sockfd) {

                    struct sockaddr_in client_addr;
                    memset(&client_addr, 0, sizeof(struct sockaddr_in));
                    socklen_t client_in = sizeof(client_addr);

                    int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, *client_in);
                    if (clientfd <= 0) continue;

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = clientfd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

                } else {

                    int clientfd = events[i].data.fd;

                    char buffer[1024] = {0};
                    int ret = recv(clientfd, buffer, 1024, 0);
                    if (ret < 0) {

                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue;
                        } else {

                        }

                        close(clientfd);

                        ev.events = EPOLLIN;
                        ev.data.fd = clientfd;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);

                    } else if (ret == 0) {

                        printf("disconnect %d\n", clientfd);

                        close(clientfd);

                        ev.events = EPOLLIN;
                        ev.data.fd = clientfd;
                        epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);
                    } else {
                        printf("Recv：%s, %d Bytes\n", buffer, ret);
                    }
                }
            }

            if (events[i].events & EPOLLOUT) {

            }
        }
    }
    
}