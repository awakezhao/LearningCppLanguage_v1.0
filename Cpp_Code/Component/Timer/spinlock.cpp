#include "spinlock.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>

#define WORKNUM 2
#define TIMES 10000
#define SPINLOCK 1

static int num = 0;

spinlock lock;
pthread_mutex_t mutex;

void* worker(void *arg) {

    for(int i = 0; i < TIMES; i++) {

#if SPINLOCK
        spinlock_lock(&lock);
        num++;
        spinlock_unlock(&lock);
#else
        pthread_mutex_lock(&mutex);
        num++;
        spinlock_unlock(&lock);
#endif
        usleep(1);
    }

    return NULL;
}

int main() 
{
    spinlock_init(&lock);
    pthread_t threadid[WORKNUM] = {0};
    for(int i = 0; i < WORKNUM; i++) {
        pthread_create(&threadid[i], NULL, worker, NULL);
    }
    
    for(int i = 0; i < 1000; i++) {

#if SPINLOCK 
        spinlock_lock(&lock);
        printf("num = %d\n", num);
        spinlock_unlock(&lock);
#else 
        pthread_mutex_lock(&mutex);
        
        printf("num = %d\n", num);
        pthread_mutex_unlock(&mutex);
#endif
        sleep(1);
        
    }
    spinlock_destroy(&lock);
    getchar();
}