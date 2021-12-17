

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

    fd_set rfds, rset;

    FZ_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    int max_fd = sockfd;
    int i = 0;

    while (1)
    {
        rset = rfds;

        int nready = select(max_fd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            printf("select error : %d\n", errno);
            continue;
        }

        if (FD_ISSET(sockfd, &rset)) {//accept
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
            socklen_t client_len = sizeof(client_addr);

            int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr);
            if (clientfd <= 0) continue;

            char str[INET_ADDRSTRLEN] = {0};
            printf("recvived from %s at port %d, sockfd:%d, clientfd:%d\n", 
                inet_ntop(AF_INET, &client_addr.sin_addr, str, sizeof(str)),
                ntohs(client_addr.sin_port), sockfd, clientfd);

            if (max_fd == FD_SETSIZE) {
                printf("clientfd --> out range\n");
                break;
            }
            FD_SET(clientfd, &rfds);

            if (clientfd > max_fd) max_fd = clientfd;

            printf("sockfd:%d, max_fd:%d, clientfd:%d\n", sockfd, max_fd, clientfd);

            if (--nready == 0) continue;
        }

        for (i = sockfd + 1; i <= max_fd; i++) {
            if (FD_ISSET(i, &rset)) {
                char buffer[BUFFER_LENGTH] = {0};
                int ret = recv(i, buffer, BUFFER_LENGTH, 0);
                if (ret < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("read all data");
                    }
                    FD_CLR(i, &rfd);
                    close(i);
                } else if (ret == 0) {
                    printf(" disconnect %d\n", i);
                    FD_CLR(i, &rfd);
                    close(i);
                } else {
                    printf("Recv: %s, %d Bytes\n", buffer, ret);
                }
                if (--nready == 0) break;
            }
        }
    }
    

    return 0;
}