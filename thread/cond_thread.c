/******************************************************************************************
 * 互斥量是防止多个线程同时操作临界区，造成线程不安全，有脏数据等。
 * 但是光使用互斥量会浪费CPU的资源，因此引入条件变量。
 *
 * 条件变量：
 *  通俗来说，设置一个条件变量让线程a阻塞在临界区之前，当其他线程给该条件变量发信号通知时，
 *  线程a才可以继续往下执行。
 *
 *  条件变量总是和互斥量一起使用，互斥量保护着条件变量，防止多个线程竞争条件变量。
 *
 ******************************************************************************************/

#include "thread.h"

/*定义互斥量*/
pthread_mutex_t mutex;
/*互斥量属性*/
pthread_mutexattr_t mutex_attr;
/*全局变量*/
int money;

/*定义条件变量并用宏初始化*/
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void err_exit(const char *err_msg)
{
     perror(err_msg);
     exit(1);
}

/*线程函数*/
void *thread_fun(void *arg)
{
     while(1)
     {
         /*加锁*/
         pthread_mutex_lock(&mutex);

         /*条件变量*/
         while(money > 0)
         {
              printf("子线程坐等money等于0...\n");
              /*阻塞直至money等于0，然后其他线程给条件变量发信号通知*/
              pthread_cond_wait(&cond, &mutex);
              /******************************************************************************
               *考虑一下为什么pthread_cond_wait函数要放在一个while循环里而不是if判断?
               *答：1.如果有多个线程都在等待这个条件变量关联的互斥量，当条件变量收到通知，
               *      他下一步就是要锁住这个互斥量，但在这个极小的时间差里面，其他线程抢先
               *      获取了这互斥量并进入临界区把数据改变了。此时条件变量应该继续判断别人
               *      刚刚抢先修改的状态，即继续执行while判断。
               *    2.防止虚假通知，收到虚假通知后，只要while里面的条件为真，就继续休眠.
               *****************************************************************************/
         }

         printf("子线程进入临界区查看money\n");
         if(money == 0)
         {
              money += 200;
              printf("子线程：money = %d\n", money);
         }

         /*解锁*/
         pthread_mutex_unlock(&mutex);

         sleep(1);
     }

     return NULL;
}

int main(int argc, char const* argv[])
{
    pthread_t tid;

    /*初始化互斥量属性*/
    if(pthread_mutexattr_init(&mutex_attr) == -1)
        err_exit("pthread_mutexattr_init ");
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

        /*如果money等于0，就通知子线程*/
        if(money == 0)
        {
             printf("通知子线程\n");
             pthread_cond_signal(&cond);
        }

        sleep(1);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    return 0;
}













