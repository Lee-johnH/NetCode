/****************************************************************
 * #include <pthread.h>
 * int pthread_cancel(pthread_t thread);
 * 被取消的线程,退出值，定义在linux的pthread库中常数PTHREAD_CANCELED的值是-1
 * 可以在头文件pthread.h中找到他的定义
 *
 * #define PTHREAD_CANCELED ((void *)-1)
 *
 * **************************************************************/
#include "thread.h"

void *thr_fn1(void *arg)
{
    printf("thread 1 returning\n");
    return (void *)1;
}

void *thr_fn2(void *arg)
{
    printf("thread 2 exiting\n");
    pthread_exit((void *)2);
}

void *thr_fn3(void *arg)
{
     while(1)
     {
         printf("thread 3 writing\n");
         sleep(1);
     }
}

int main(int argc, char const* argv[])
{
    pthread_t tid;
    void *tret;

    pthread_create(&tid, NULL, thr_fn1, NULL);
    pthread_join(tid, &tret);
    printf("thread 1 exit code %d\n", (int)tret);

    pthread_create(&tid, NULL, thr_fn2, NULL);
    pthread_join(tid, &tret);
    printf("thread 2 exit code %d\n", (int)tret);

    pthread_create(&tid, NULL, thr_fn3, NULL);
    sleep(3);
    pthread_cancel(tid);
    pthread_join(tid, &tret);
    printf("thread 3 exit code %d\n", (int)tret);

    return 0;
}
