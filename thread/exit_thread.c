#include "thread.h"

void *thread_fun(void *arg)
{
    //如果传参数1，那么就采用return方式退出
    if(strcmp("1", (char *)arg) == 0)
    {
        printf("new thread return!\n");
        return (void *)1;
    }
    //如果传参数2，那么就采用pthread_exit方式退出
    if(strcmp("2", (char *)arg) == 0)
    {
        printf("new thread pthread_exit!\n");
        pthread_exit((void *)2);
    }
    //如果传参数3，那么就采用exit方式退出
    if(strcmp("3", (char *)arg) == 0)
    {
        printf("new thread exit!\n");
        exit(3);
    }

}

int main(int argc, char const* argv[])
{
    int err;
    pthread_t tid;

    err = pthread_create(&tid, NULL, thread_fun, (void *)argv[1]);
    if(err != 0)
    {
        printf("create new thread failed\n");
        return 0;
    }

    sleep(1);
    printf("main thread!\n");

    return 0;
}
