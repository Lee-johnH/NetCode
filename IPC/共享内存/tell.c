#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tell.h"

static int fd[2];

//管道初始化
void init()
{
    if(pipe(fd) < 0)
    {
        perror("pipe error!");
        exit(1);
    }
}

//利用管道进行等待
void wait_pipe()
{
    char c;
    //管道读写默认是阻塞性的
    if(read(fd[0], &c, 1) < 0)
    {
        perror("wait pipe error!");
        exit(1);
    }
}

//利用管道进行通知
void notify_pipe()
{
    char c = 'c';
    if(write(fd[1], &c, 1) < 0)
    {
        perror("notify pipe error!");
        exit(1);
    }
}

//销毁管道
void destory_pipe()
{
    close(fd[0]);
    close(fd[1]);
}


