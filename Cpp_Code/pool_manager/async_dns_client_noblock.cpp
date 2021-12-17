

typedef void(*async_result_cb)(struct dns_item *arg, int count);

struct async_context {
    int epfd;
    pthread_t threadid;
};

struct ep_arg {
    int sockfd;
    async_result_cb cb;
};

#define ASYNC_EVENTS 128

void *dns_async_callback(void *arg){

    struct async_context *ctx = (struct async_context*)arg;

    for(;;) {

        struct epoll_event events[ASYNC_EVENTS] = {0};
        
        int nready = epoll_wait(ctx->epfd, events, ASYNC_EVENTS, -1);
        if (nready < 0) {
            continue;
        }

        for (int i = 0; i < nready; i++) {

            struct ep_arg *ptr = events[i].data.ptr;
            int sockfd = ptr->sockfd;

            char buffer[1024] = {0};
            struct sockaddr_in addr;
            size_t addr_len = sizeof(struct sockaddr_in);

            int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, (socklen_t&)&addr_len);

            struct dns_item *domains = NULL;
            int count = dns_parse_response(buffer, &domains);

            ptr->cb(domains, count);

            close(sockfd);
            free(ptr);
        }
    }
}

struct async_context* dns_async_client_init(void) {

    int epfd = epoll_create(1);
    if (epfd < 0) return NULL;

    struct async_context* ctx = calloc(1, sizeof(struct async_context));
    if (ctx == NULL) return NULL;

    ctx->epfd = epfd;

    int ret = pthread_create(&ctx->threadid, NULL, dns_async_callback, ctx);
    if (ret) {
        close(epfd);
        free(ctx);
        return NULL;
    }

    return ctx;
}

int dns_async_client_destroy(struct async_context* ctx) {

    close(ctx->epfd);
    pthread_cancel(ctx->threadid);

}

int dns_async_client_commit(struct async_context *ctx, async_result_cb cb) {

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 创建fd
    if (sockfd < 0) {
        perror("create socket failed\n");
        exit(-1);
    }

    struct sockaddr_in dest;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(DNS_SVR);

    int ret = connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));  // 连接，可选的

    // 添加应用协议
    struct dns_header header = {0};
    dns_create_header(&header);

    struct dns_question question = {0};
    dns_create_question(&question, domain);

    char request[1024] = {0};
    int req_len = dns_build_request(&header, &question, request);

    // 发送数据
    int slen = sendto(sockfd, request, req_len, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr));

    // 将fd添加到epoll
    struct ep_arg *ptr = calloc(1, sizeof(struct ep_arg));
    if (ptr == NULL) return -1;
    ptr->sockfd = sockfd;
    ptr->cb = cb;

    struct epoll_event ev;
    ev.data.ptr = ptr;
    ev.events = EPOLLIN;
    epoll_ctl(ctx->epfd, EPOLL_CTL_ADD, sockfd, &ev);

    return 0;
}