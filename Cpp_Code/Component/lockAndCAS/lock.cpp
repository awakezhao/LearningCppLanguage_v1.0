#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/mman.h>

#define THREA_SIZE 10

int count = 0;
pthread_mutex_t mutex;
pthread_spinlock_t spinlock;
pthread_rwlock_t rwlock;

int inc(int *value, int add) {

    int old;

    __asm__ volatile (
        "lock; xaddl %2, %1;"
        : "=a" (old)
        : "m" (*value), "a" (add)
        : "cc", "memory"
    );

    return old;
}

void *func(void *arg) {
    int *pcount = (int *)arg;

    int i = 0;
    while(i++ < 10000) {
#if 0
        (*pcount)++; //无锁
#elif 0
        pthread_mutex_lock(&mutex);
        (*pcount)++;
        pthread_mutex_unlock(&mutex);
#elif 0
        if (0 != pthread_mutex_trylock(&mutex)) {
            i--;
            continue;
        }
        (*pcount)++;
        pthread_mutex_unlock(&mutex);
#elif 0
        pthread_spin_lock(&spinlock);
        (*pcount)++;
        pthread_spin_unlock(&spinlock);
#elif 0
        pthread_rwlock_wrlock(&rwlock);
        (*pcount)++;        
        pthread_rwlock_unlock(&rwlock);
#elif 1
        inc(pcount, 1);
#endif
        usleep(1);
    }

    return NULL;
}

int main() {
#if 0
    pthread_t threadid[THREA_SIZE] = {0};

    pthread_mutex_init(&mutex, NULL);
    pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init(&rwlock, NULL);

    int i = 0;
    int count = 0;
    for (i = 0; i < THREA_SIZE; i++) {
        int ret = pthread_create(&threadid[i], NULL, func, &count);
        if (ret) break;
    }

    for(i = 0; i < 100; i++) {
        pthread_rwlock_rdlock(&rwlock);
        printf("count --> %d\n", count);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
#else

    int *pcount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);

    int i = 0;
    pid_t pid = 0;
    for (i = 0; i < THREA_SIZE; i++) {

        /*
            fork在父进程中返回子进程id，可用来等待子进程结束: waitpid。
            在子进程中返回0。
            根据写入时复制语义，物理内存不需要被实际复制。
            取而代之的是，两个进程的虚拟内存页面可能指向物理内存中的同一个页，
            直到它们写入该页时，写入才会发生。
        */
        pid = fork();  
        if (pid <= 0) {
            usleep(1);
            break;
        }
    }

    if (pid > 0) {
        for (i = 0; i < 100; i++) {
            printf("count --> %d\n", (*pcount));
            sleep(1);
        }
    } else {
        int i = 0;
        while (i++ < 10000) {
            inc(pcount, 1);
            usleep(1);
        }
    }

#endif
}