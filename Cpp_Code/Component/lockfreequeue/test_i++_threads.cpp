

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_NUMBER 2
#define THREAD_INCR_COUNT 500000
static int s_i = 0;

static int lxx_atomic_add(int *ptr, int inncremennt) {

    int old_value = *ptr;
    __asm__ volatile (
        "lock; xadd %0, %1 \n\t"
        : "=r"(old_value), "=m"(*ptr)
        : "0"(inncremennt), "m"(*ptr)
        : "cc", "memory"
    );

    return *ptr;
}

void *thread_increase(void *arg) {

    for(int i = 0; i < THREAD_INCR_COUNT; i++) {
        // ++s_i;
        lxx_atomic_add(&s_i, 1);
    }
    printf("finish\n");
    pthread_exit(NULL);

}

int main() {

    pthread_t threadArr[THREAD_NUMBER];

    for(int i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&threadArr[i], NULL, thread_increase, NULL);
    }

    for(int i = 0; i < THREAD_NUMBER; i++) {
        pthread_join(threadArr[i], NULL);
    }

    printf("s_i actual:%d, expected:%d\n", s_i, THREAD_NUMBER * THREAD_INCR_COUNT);

    return 0;
}