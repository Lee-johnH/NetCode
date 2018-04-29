#include "thread.h"

/*定义互斥锁*/
pthread_mutex_t mutex;
/*互斥量属性*/
pthread_mutexattr_t mutex_attr;
/*全局变量*/
int money;

void err_exit(const char *err_msg)
{
    printf("error:%s\n", err_msg);
    exit(1);
}

/*线程函数*/
void *thread_fun(void *arg)
{
    while(1)
    {
         /*加锁*/
        pthread_mutex_lock(&mutex);

        printf("子线程进入临界区查看money\n");

        if(money == 0)
        {
             money += 200;
             printf("子线程：money = %d\n", money);
        }

        /*解锁*/
        pthread_mutex_unlock(&mutex);

        /*防止独占CPU*/
        sleep(1);
    }

    return NULL;
}

int main(int argc, char const* argv[])
{
    pthread_t tid;

    /*初始化互斥锁属性*/
    if(pthread_mutexattr_init(&mutex_attr) == -1)
        err_exit("pthread_mutexattr_init");

    /*设置互斥量属性*/
    if(pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL) == -1)
        err_exit("pthread_mutexattr_settype ");

    /*初始化互斥量*/
    if(pthread_mutex_init(&mutex, &mutex_attr) == -1)
        err_exit("pthread_mutex_init ");

    /*创建一个线程*/
    if(pthread_create(&tid, NULL, thread_fun, NULL) == -1)
        err_exit("pthread_create ");

    money = 1000;

    while(1)
    {
         /*加锁*/
        pthread_mutex_lock(&mutex);

        if(money > 0)
        {
            money -= 100;
            printf("主线程：money = %d\n", money);
        }

        /*解锁*/
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}









