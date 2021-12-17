#include <stdio.h>
#include <time.h>
#include <windows.h>

#define uint8_t unsigned char
#define uint32_t unsigned int
#define int32_t int

#define MAX_TIMER ((1<<17)-1)
#define MAX_CONN ((1<<16)-1)

typedef struct conn_node {
    uint8_t used;
    int id;
} conn_node_t;

typedef struct timer_node {
    struct timer_node *next;
    struct conn_node *node;
    uint32_t idx;
} timer_node_t;

// 内存池 预先分配内存 供后面使用
static timer_node_t timer_nodes[MAX_TIMER] = {0};
static conn_node_t conn_nodes[MAX_CONN] = {0};
static uint32_t t_iter = 0;
static uint32_t c_iter = 0;

timer_node_t *get_timer_node() {
    t_iter++;
    while (timer_nodes[t_iter & MAX_TIMER].idx > 0)
    {
        t_iter++;
    }
    
    timer_nodes[t_iter].idx = t_iter;
    return &timer_nodes[t_iter];
}

conn_node_t *get_conn_node() {
    c_iter++;
    while (conn_nodes[c_iter & MAX_CONN].used > 0)
    {
        c_iter++;
    }
    return &(conn_nodes[c_iter]);
}

#define TW_SIZE 16
#define EXPIRE 10
#define TW_MASK (TW_SIZE - 1)   // 求mod 转化为位与mask
static uint32_t tick = 0;

typedef struct link_list {
    timer_node_t head;
    timer_node_t *tail;
} link_list_t;

void add_conn(link_list_t *tw, conn_node_t *cnode, int delay) {
    link_list_t *list = &tw[(tick + EXPIRE + delay) & TW_MASK];
    timer_node_t *tnode = get_timer_node();
    cnode->used++;
    tnode->node = cnode;
    list->tail->next = tnode;
    list->tail = tnode;
    tnode->next = NULL;
}

void link_clear(link_list_t *list) {
    list->head.next = NULL;
    list->tail = &(list->head);
}

void check_conn(link_list_t *tw) {
    int32_t itick = tick;
    tick++;
    link_list_t *list = &tw[itick & TW_MASK];
    timer_node_t *current = list->head.next;
    while (current) {
        timer_node_t *temp = current;
        current = current->next;
        conn_node_t *cn = temp->node;
        cn->used--;
        temp->idx = 0;
        if (cn->used == 0) {
            printf("fd:%d kill down\n", cn->id);
            temp->next = NULL;
            continue;
        }
        printf("fd:%d used:%d\n", cn->id, cn->used);
    }
    link_clear(list);
}

static uint32_t 
current_time() {
#if 0
    struct timespec ti;
    clock_gettime(CLOCK_MONOTONIC, &ti);
    t = (uint32_t)ti.tv_sec * 1000;
    t += ti.tv_snec / 1000000;
#elif 0
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (uint32_t)ti.tv_sec * 1000;
    t += tv.tv_usec / 1000;
#else
    time_t t = time(0);
    localtime(&t);
    t *= 1000;
#endif
    return t;
}


int main() {
    memset(timer_nodes, 0, MAX_TIMER * sizeof(timer_node_t));
    memset(conn_nodes, 0, MAX_CONN * sizeof(conn_node_t));

    link_list_t tw[TW_SIZE];
    memset(tw, 0, TW_SIZE * sizeof(link_list_t));
    for (int i = 0; i < TW_SIZE; i++) {
        link_clear(&tw[i]);
    }

    // 该测试起始时间为0秒，所以 delay 不能添加超过10的数。
    {
        conn_node_t *node = get_conn_node();
        node->id = 10001;
        add_conn(tw, node, 0);
        add_conn(tw, node, 5);
        add_conn(tw, node, 9);
    }

    {
        conn_node_t *node = get_conn_node();
        node->id = 10002;
        add_conn(tw, node, 0);
        add_conn(tw, node, 7);
    }

    {
        conn_node_t *node = get_conn_node();
        node->id = 10003;
        add_conn(tw, node, 3);
    }

    time_t start = current_time();
    for (;;) {
        time_t now = current_time();
        if (now - start > 0) {
            for (int i=0; i<now-start; i++)
                check_conn(tw);
            start = now;
            printf("check conn tick:%d\n", tick);
        }
        Sleep(20);
    }

}