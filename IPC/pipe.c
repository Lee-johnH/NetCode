#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define MAXLINE 80

int main(int argc, char const* argv[])
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];

    if(pipe(fd) < 0)
    {
        perror("pipe");
        exit(1);
    }

    if((pid = fork()) < 0)
    {
        perror("fork");
        exit(1);
    }

    if(pid > 0)     /*父写*/
    {
         close(fd[0]);
         write(fd[1], "hello world\n", 12);
         wait(NULL);
    }
    else            /*子读*/
    {
         close(fd[1]);
         n = read(fd[0], line, MAXLINE);
         write(STDOUT_FILENO, line, n);
    }

    return 0;
}
