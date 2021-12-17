
#define ngx_rgtree_t
#define ngx_rbtree_node_t

ngx_rbtree_t timer;
static ngx_rbtree_node_t sentinel;

typedef struct timer_entry_s timer_entry_t;
typedef void (*timer_handler_pt)(timer_entry_t *ev)

struct timer_entry_s {
    ngx_rbtree_node_t timer;
    timer_handler_pt handler;
};

static uint32_t 
current_time() {
    uint32_t t;
#if 0
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    t = (uint32_t)ti.tv_sec * 1000;
    t += ti.tv_snec / 1000000;
#else 
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (uint32_t)ti.tv_sec * 1000;
    t += tv.tv_usec / 1000;
#endif
    return t;
}

int init_timer() {
    ngx_rbtree_init(&timer, &sentinel, ngx_rbtree_insert_timer_value);
    return 0;
}

void add_timer(timer_entry_t *te, uint32_t msec) {
    msec += current_time();
    te->timer.key = msec;
    ngx_rbtree_insert(&timer, &te->timer);
}

void del_timer(timer_entry_t *te) {
    ngx_rbtree_delete(&timer, &te->timer);
}

void expire_timer() {
    timer_entry_t *te;
    ngx_rbtree_node_t *sentinel, *root, *node;
    sentinel = timer.sentinel;
    uint32_t now = current_time();
    for (;;) {
        root = timer.root;
        if (root == sentinel) break;
        node = ngx_rbtree_min(root, sentinel);
        if (node->key > now) break;
        te = (timer_entry_t *) ((char *)node - offsetof(timer_entry_t, timer));
        te->handler(te);
        ngx_rbtree_delete(&timer, &te->timer);
        free(te);
    }
}