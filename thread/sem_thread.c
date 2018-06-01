/*
 * 线程的信号量
 */

#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

//定义线程信号量
sem_t sem1;
sem_t sem2;

void* a_fun(void *arg)
{
    sem_wait(&sem1);
    printf("thread a running\n");

    return (void*)0;
}


void* b_fun(void *arg)
{
    sem_wait(&sem2);
    printf("thread b running\n");
    //释放线程a(对线程信号量sem1作加1操作，让阻塞的线程a继续运行)
    sem_post(&sem1);

    return (void*)0;
}

void* c_fun(void *arg)
{
    printf("thread c running\n");
    //释放线程b(对线程信号量sem2加1操作，让阻塞的线程b继续运行)
    sem_post(&sem2);

    return (void*)0;
}

int main(void)
{
    pthread_t a, b, c;
    //线程信号量初始化，初始值为0
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);

    pthread_create(&a, NULL, a_fun, (void*)0);
    pthread_create(&b, NULL, b_fun, (void*)0);
    pthread_create(&c, NULL, c_fun, (void*)0);

    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);

    //销毁线程信号量
    sem_destroy(&sem1);
    sem_destroy(&sem2);

    return 0;
}
