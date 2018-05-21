#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * 模仿命令: cat /etc/passwd |grep "root"
 *
 */

char *cmd1[3] = {"/bin/cat", "/etc/passwd", NULL};
char *cmd2[3] = {"/bin/grep","root", NULL};

int main(void)
{
    int fd[2];
    int i = 0;
    pid_t pid;

    if(pipe(fd) < 0)
    {
        perror("pipe error!");
        exit(1);
    }

    for(i = 0; i < 2; i++)
    {
        pid = fork();
        if(pid < 0)
        {
            perror("fork error!");
            exit(1);
        }
        else if(pid == 0)   //子进程
        {
            if(i == 0)  //第一个子进程负责往管道里写cat /etc/passwd命令的结果
            {
                close(fd[0]);
                //将标准输出重定向到管道的写端
                if(dup2(fd[1], STDOUT_FILENO) < 0)
                {
                    perror("dup2 error!");
                    exit(1);
                }
                close(fd[1]);

                //调用exec函数执行cat命令
                if(execvp(cmd1[0], cmd1) < 0)
                {
                    perror("execvp error!");
                    exit(1);
                }
                break;

            }
            else        //第二个子进程负责从管道读cat /etc/passwd命令的结果
            {
                usleep(500);
                close(fd[1]);
                //将标准输入重定向到管道的读端
                if(dup2(fd[0], STDIN_FILENO) < 0)
                {
                    perror("dup2 error!");
                    exit(1);
                }
                close(fd[0]);

                //调用execvp执行grep命令
                if(execvp(cmd2[0], cmd2) < 0)
                {
                    perror("execvp error!");
                    exit(1);
                }
                break;

            }
        }
        else            //父进程
        {
            if(i == 1)
            {
                close(fd[0]);
                close(fd[1]);
                wait(0);
                wait(0);
            }
        }
    }



    return 0;
}
