
#define POLL_SIZE 1024
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

    struct pollfd fds[POLL_SIZE] = {0};
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    int max_fd = 0, i = 0;
    for (i = 1; i < POLL_SIZE; i++) {
        fds[i].fd = -1;
    }

    while (1)
    {
        int nready = poll(fds, max_fd+1, 5);
        if (nready <= 0) continue;

        if ((fds[0].revents & POLLIN) == POLLIN) {

            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(struct sockaddr_in));
            socklen_t client_len = sizeof(client_addr);

            int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
            if (clientfd <= 0) continue;

            fds[clientfd].fd = clientfd;
            fds[clientfd].events = POLLIN;

            if (clientfd > max_fd) max_fd = clientfd;

            if (--nready == 0) continue;
        }

        for (i = sockfd + 1; i <= max_fd; i++) {
            if (fds[i].revents & (POLLIN|POLLERR)) {
                char buffer[BUFFER_LENGTH] = {0};
                int ret = recv(i, buffer, BUFFER_LENGTH, 0);
                if (ret < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        printf("read all data");
                    }
                    close(i);
                    fds[i].fd = -1;
                } else if (ret == 0) {
                    printf(" disconnect %d\n", i);

                    close(i);
                    fds[i].fd = -1;
                    break;
                } else {
                    printf("Recv: %s, %d Bytes\n", buffer, ret);
                }
                if (--nready == 0) break;
            }
        }
    }
    

    return 0;
}