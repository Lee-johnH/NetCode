#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define MAXLINE 512

ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while(nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0 && errno == EINTR)
                nwritten = 0;   //call write() again
            else
                return (-1);
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return (n);
}

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);

    if(n < 0 && errno == EINTR)
        goto again;
    else if(n < 0)
        perror("str_echo: read error");
}

int main(int argc, char const* argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;   //ipv4套接字地址
    int portnumber;

    if(argc != 2)
    {
        fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
        exit(1);
    }

    if((portnumber = atoi(argv[1])) < 0)
    {
         fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
         exit(1);
    }

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
         fprintf(stderr, "Socket error: %s\n\a", strerror(errno));
         exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portnumber);

    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
        fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
        exit(1);
    }

    if(listen(listenfd, 5) == -1)
    {
        fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
        exit(1);
    }

    for(;;)
    {
         clilen = sizeof(cliaddr);
         if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1)
         {
              fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
              exit(1);
         }

         if((childpid = fork()) == 0)
         {
              close(listenfd);
              str_echo(connfd);
              exit(0);
         }

         close(connfd);
    }

    return 0;
}
