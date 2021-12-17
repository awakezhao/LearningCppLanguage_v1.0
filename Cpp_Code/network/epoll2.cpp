

#define EPOLL_SZIE 1024
#define BUFFER_LENGTH 1024

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Paramter Error\n");
        return -1;
    }
    int port = atoi(argv[1]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("bind");
        return -2;
    }

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        return -3;
    }

    int epoll_fd = epoll_create(EPOLL_SIZE);
    struct epoll_event ev, events[EPOLL_SZIE] = {0};

    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);

    while (1)
    {
        
        int nready = epoll_wait(epoll_fd, events, EPOLL_SZIE, -1);
        if (nready == -1) {
            printf("epoll_wait\n");
            break;
        }

        int i = 0;
        for (i = 0; i < nready; i++) {
            if (events[i].data.fd == sockfd) {
                
                struct sockaddr_in client_addr;
                memset(&client_addr, 0, sizeof(struct sockaddr_in));
                socklen_t client_len = sizeof(client_addr);

                int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
                if (clientfd <= 0) continue;

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clientfd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientfd, &ev);
            } else {

                if (events[i].events & EPOLLIN) { // 

					int clientfd = events[i].data.fd;

					char buffer[1024] = {0};
					int ret = recv(clientfd, buffer, 5, 0);
					if (ret < 0) {

						if (errno == EAGAIN || errno == EWOULDBLOCK) { //
							continue;
						} else {
							
						}

						close(clientfd);

						ev.events = EPOLLIN;
						ev.data.fd = clientfd;
						epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);

					} else if (ret == 0) { //

						// 
						printf("disconnect %d\n", clientfd);

						close(clientfd);

						ev.events = EPOLLIN;
						ev.data.fd = clientfd;
						epoll_ctl(epfd, EPOLL_CTL_DEL, clientfd, &ev);
						
					} else {

						printf("Recv: %s, %d Bytes\n", buffer, ret);

					}
					
				} 

				if (events[i].events & EPOLLOUT) {

					int clientfd = events[i].data.fd;
					char buffer[1024] = {0};
					
					//send(clientfd, )

				}
            }
        }
    }

    return 0;
}