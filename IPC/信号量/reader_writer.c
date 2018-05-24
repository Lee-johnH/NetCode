#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//读者和写者共享的资源
typedef struct{
    int val;
    int semid;
}Strorage;

void per_exit(const char *mess)
{
    perror(mess);
    exit(1);
}

void init(Strorage *s)
{
    assert(s != NULL);
    //创建信号量集（包含两个信号量）
    if((s->semid = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0777)) < 0)
        per_exit("semget error!");
    //对信号量集中的所有信号量初始化
    union semun{
        int                 val;
        struct semid_ds     *ds;
        unsigned short      *array;
    };
    union semun un;
    //2个信号量的初值设置为0
    unsigned short array[2] = {0, 0};
    un.array = array;
    if(semctl(s->semid, 0, SETALL, un) < 0)
        per_exit("semctl error!");

}

void destroy(Strorage *s)
{
    assert(s != NULL);

    if(semctl(s->semid, 0, IPC_RMID, NULL) < 0)
        per_exit("semctl error!");

}

void write(Strorage *s, int val)
{
    //写入数据到Storge
    s->val = val;
    printf("%d write %d\n", getpid(), val);

    //设置0号信号量(s1)作V(1)操作
    struct sembuf ops_v[1] = {{0, 1, SEM_UNDO}};
    //设置1号信号量(s2)作P(1)操作
    struct sembuf ops_p[1] = {{1, -1, SEM_UNDO}};

    //V(s1)
    if(semop(s->semid, ops_v, 1) < 0)   //0号信号量作V(1)
        per_exit("semop error!");

    //P(s2)
    if(semop(s->semid, ops_p, 1) < 0)   //1号信号量作P(1)
        per_exit("semop error!");

}

void read(Strorage *s)
{
    assert(s != NULL);

    //设置0号信号量(s1)作P(1)操作
    struct sembuf ops_p[1] = {{0, -1, SEM_UNDO}};
    //设置1号信号量(s2)作V(1)操作
    struct sembuf ops_v[1] = {{1, 1, SEM_UNDO}};

    //P(s1);
    if(semop(s->semid, ops_p, 1) < 0)   //0号信号量作P(1)
        per_exit("semctl error!");
    //从Storage中读取数据
    printf("%d read %d\n", getpid(), s->val);

    //V(s2);
    if(semop(s->semid, ops_v, 1) < 0)   //1号信号量作V(1)
        per_exit("semctl error!");
}

int main(void)
{
    //将共享资源Storage创建在共享内存中
    int shmid;
    if((shmid = shmget(IPC_PRIVATE, sizeof(Strorage), IPC_CREAT | IPC_EXCL | 0777)) < 0)
        per_exit("shmget error!");

    //父进程进行共享内存映射
    Strorage *s = (Strorage *)shmat(shmid, 0, 0);
    if(s == (Strorage *)-1)
        per_exit("shmat error!");

    //创建信号量集并初始化
    init(s);

    pid_t pid;
    if((pid = fork()) < 0)
    {
        per_exit("fork error!");
    }
    else if(pid > 0)    //父进程(写者)
    {
        int i = 1;
        for(i = 1; i <= 100; i++)
        {
            write(s, 1);
        }
        wait(0);
        destroy(s);
        shmdt(s);
        shmctl(shmid, IPC_RMID, NULL);
    }
    else    //子进程(读者)
    {
        int i = 1;
        for(; i<= 100; i++)
        {
            read(s);
        }
        shmdt(s);
    }

    return 0;
}
