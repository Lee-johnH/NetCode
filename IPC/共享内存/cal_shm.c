#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tell.h"
#include <sys/wait.h>

void per_exit(char *msg)
{
    perror(msg);
    exit(1);
}

int main(void)
{
    //创建共享内存
    int shmid;
    if((shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | IPC_EXCL | 0777)) < 0)
        per_exit("shmget error!");

    pid_t pid;
    init();     //初始化管道
    if((pid = fork()) < 0)
        per_exit("fork error!");
    else if(pid > 0)    //父进程
    {
        //父进程进行共享内存的映射
        int *pi = (int *)shmat(shmid, 0, 0);
        if(pi == (int *)-1)
            per_exit("shmat error!");

        //往共享内存中写入数据(通过操作映射的地址即可)
        *pi = 100;
        *(pi + 1) = 200;

        //操作完毕解除共享内存的映射
        shmdt(pi);

        //通知子进程读数据
        notify_pipe();
        destory_pipe();
        wait(0);
    }
    else    //子进程
    {
        //子进程阻塞，等待父进程先往共享内存写数据
        wait_pipe();
        /*子进程从共享内存读取数据*/

        //子进程进行共享内存的映射
        int *pi = (int *)shmat(shmid, 0, 0);
        if(pi == (int *)-1)
            per_exit("shmat error!");

        printf("start: %d, end: %d\n", *pi, *(pi+1));

        //读取完毕后解除映射
        shmdt(pi);
        //删除共享内存
        shmctl(shmid, IPC_RMID, NULL);

        destory_pipe();
    }

    return 0;
}
