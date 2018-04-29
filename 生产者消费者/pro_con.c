#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE 8

struct pro_con_queue
{
    int buffer[BUFFER_SIZE];
    int readpos, writepos;  //队列的读指针位置和写指针位置
    pthread_mutex_t mutex;
    pthread_cond_t notempty, notfull;   //缓冲区非空，非满状态判断
};

char *get_time(void)
{
    time_t ntime;
    time(&ntime);

    return asctime(localtime(&ntime));
}

void init(struct pro_con_queue *tmp)
{
    pthread_mutex_init(&tmp->mutex, NULL);
    pthread_cond_init(&tmp->notempty, NULL);
    pthread_cond_init(&tmp->notfull, NULL);

    tmp->readpos = 0;
    tmp->writepos = 0;
}

void pro_data(struct pro_con_queue *tmp, int data)
{
     pthread_mutex_lock(&tmp->mutex);
     /*判断队列是否满*/
     if((tmp->writepos + 1) % BUFFER_SIZE == tmp->readpos)
     {
         pthread_cond_wait(&tmp->notfull, &tmp->mutex);
     }

     //pthread_mutex_lock(&tmp->mutex);
     //仔细理解互斥锁的位置
     tmp->buffer[tmp->writepos] = data;
     tmp->writepos++;
     if(tmp->writepos >= BUFFER_SIZE)
         tmp->writepos = 0;

     printf("put [%d] in the queue at %s \n", data, get_time());

     pthread_mutex_unlock(&tmp->mutex);
     pthread_cond_signal(&tmp->notempty);

}

int con_data(struct pro_con_queue *tmp)
{
     int data;

     pthread_mutex_lock(&tmp->mutex);
     if(tmp->readpos == tmp->writepos)
     {
         pthread_cond_wait(&tmp->notempty, &tmp->mutex);
     }

     data = tmp->buffer[tmp->readpos];
     tmp->readpos++;

     if(tmp->readpos >= BUFFER_SIZE)
         tmp->readpos = 0;

     printf("get the data [%d] in the queue \n", data);
     pthread_cond_signal(&tmp->notfull);
     pthread_mutex_unlock(&tmp->mutex);

     return data;
}

#define OVER (-1)

struct pro_con_queue buffer;

void *producer(void *data)
{
     int n;

     for(n = 0; n < 10; n++)
     {
         printf("the %d produce \n", n);
         pro_data(&buffer, n);
     }

     pro_data(&buffer, OVER);

}

void *consumer(void *data)
{
     int d;

     while(1)
     {
         d = con_data(&buffer);

         if(d == OVER)
             break;
         printf("consumerd the num is %d\n", d);
     }
}

int main(int argc, char const* argv[])
{
    pthread_t th_1, th_2;
    pthread_attr_t attr;
    void *retval;

    init(&buffer);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&th_2, NULL, consumer, NULL);
    //pthread_create(&th_2, &attr, consumer, NULL);

    sleep(2);   //先生产数据到队列中

    pthread_create(&th_1, NULL, producer, NULL);

    pthread_join(th_1, &retval);
    pthread_join(th_2, &retval);


    return 0;
}







