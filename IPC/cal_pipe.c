#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 *父进程通过管道传输两个数据给子进程
 *由子进程负责从管道中读取并输出
 *
 */

int main()
{
    int pid;
    int fd[2];
    //创建管道
    if(pipe(fd) < 0)
    {
        perror("pipe error!");
        exit(1);
    }

    if((pid = fork()) < 0)
    {
        perror("fork() error!");
        exit(1);
    }
    else if(pid > 0)   //父进程写管道
    {
        close(fd[0]);
        int wStart = 0, wEnd = 100;
        if(write(fd[1], &wStart, sizeof(wStart)) == -1)
        {
            perror("write error!");
            exit(1);
        }
        if(write(fd[1], &wEnd, sizeof(wEnd)) == -1)
        {
            perror("write error!");
            exit(1);
        }
        close(fd[1]);
        wait(0);
    }
    else            //子进程读管道
    {
        close(fd[1]);
        int rStart, rEnd;
        if(read(fd[0], &rStart, sizeof(rStart)) == -1)
        {
            perror("read error!");
            exit(1);
        }
        if(read(fd[0], &rEnd, sizeof(rEnd)) == -1)
        {
            perror("read error!");
            exit(1);
        }
        close(fd[0]);

        fprintf(stdout, "child get data from parent: start:%d \tend:%d\n", rStart, rEnd);
    }


    return 0;
}
