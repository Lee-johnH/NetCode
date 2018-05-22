#include "account.h"
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

void per_exit(char *msg)
{
    perror(msg);
    exit(1);
}

int main(void)
{
    //在共享内存中创建银行账户
    int shmid;
    if((shmid = shmget(IPC_PRIVATE, sizeof(Account), IPC_CREAT | IPC_EXCL | 0777)) < 0)
        per_exit("shmget error!");

    //进程共享内存映射(a为映射的地址)
    Account *a = (Account *)shmat(shmid, 0, 0);
    if(a == (Account*)-1)
        per_exit("shmat error!");

    a->code = 10001;
    a->balance = 10000;
    printf("balance: %f\n", a->balance);

    pid_t pid;
    if((pid = fork()) < 0)
        per_exit("fork error!");
    else if(pid > 0)    //父进程
    {
        double amt = withdraw(a, 10000);
        printf("pid: %d withdraw %f from code %d\n", getpid(), amt, a->code);
        wait(0);

        //对共享内存的操作要在解除映射之前
        printf("balance: %f\n", a->balance);

        shmdt(a);
    }
    else    //子进程
    {
        //子进程也执行取款操作
        double amt = withdraw(a, 10000);
        printf("pid: %d withdraw %f from code %d\n", getpid(), amt, a->code);
        shmdt(a);
    }

    return 0;
}
