#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>
#include "pv.h"

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

//创建信号量集合，并初始化semnums个信号灯的值(value)
int I(int semnums, int value)
{
    //创建信号量集
    int semid;
    semid = semget(IPC_PRIVATE, semnums, IPC_CREAT | IPC_EXCL | 0777);
    if(semid < 0)
    {
        return -1;
    }

    union semun un;
    unsigned short *array = (unsigned short *)calloc(semnums, sizeof(unsigned short));

    int i;
    for(i = 0; i < semnums; i++)
    {
        array[i] = value;
    }
    un.array = array;
    /*
     * 初始化信号量集中所有信号灯的初值
     * 0:表示要初始化所有的信号灯
     */
    if(semctl(semid, 0, SETALL, un) < 0)
    {
        perror("semctl error");
        return -1;
    }
    free(array);
    return semid;
}

//对信号量集(semid)中的信号灯(semnum)作P(value)操作
void P(int semid, int semnum,int value)
{
    assert(value >= 0);
    //定义sembuf类型的结构体数组，放置若干个结构体变量
    //放置要操作的信号量、要作的P或V操作
    struct sembuf ops[] =
    {
        {semnum, -value, SEM_UNDO}
    };

    if(semop(semid, ops, sizeof(ops) / sizeof(struct sembuf)) < 0)
    {
        perror("semop error!");
        //不退出，只提示
    }

}

//对信号量集(semid)中的信号灯(semnum)作V(value)操作
void V(int semid, int semnum, int value)
{
    assert(value >= 0);
    struct sembuf ops[] =
    {
        {semnum, value, SEM_UNDO}
    };

    if(semop(semid, ops, sizeof(ops) / sizeof(struct sembuf)) < 0)
    {
        perror("semop error!");
        //不退出，只提示
    }

}

//销毁信号量集(semid)
void D(int semid)
{
    if(semctl(semid, 0, IPC_RMID, NULL) < 0)
    {
        perror("semctl error!");
        exit(1);
    }

}


