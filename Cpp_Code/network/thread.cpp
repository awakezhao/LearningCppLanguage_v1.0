

#define BUFFER_LENGTH 1024

void *client_callback(void *arg) {
    int clientfd = *(int *)arg;

    while (1)
    {
        char buffer[BUFFER_LENGTH] = {0};
        int ret = recv(clientfd, buffer, BUFFER_LENGTH, 0);
        if (ret < 0) {
            if (errno == GAGAIN || errno == EWOULDBLOCK) {
                printf("read all data\n");
            }
            close(clientfd);
            return NULL;
        } else if (ret == 0) {
            printf("disconnect \n");
            close(clientfd);
            return NULL;
        } else {
            printf("Recv:%s, %d Bytes\n", buffer, ret);
            return NULL;
        }
    }
    
}

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

    while (1)
    {
        struct sockaddr_in client_addr = {0};
        socklen_t client_len = sizeof(client_addr);

        int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (clientfd <= 0) continue;

        pthread_t thread_id;
        int ret = pthread_create(&thread_id, NULL, client_callback, &clientfd);
        if (ret < 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    return 0;
}