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

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for(n = 1; n < maxlen; n++)
    {
again:
        if((rc = read(fd, &c, 1)) == 1)
        {
             *ptr++ = c;
             if(c == '\n')
                 break;
        }
        else if(rc == 0)
        {
             *ptr = 0;
             return (n - 1);
        }
        else
        {
             if(errno == EINTR)
                 goto again;
             return (-1);
        }
    }

    *ptr = 0;
    return (n);
}

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

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];

    while(fgets(sendline, MAXLINE, fp) != NULL)
    {
        writen(sockfd, sendline, strlen(sendline));

        if(readline(sockfd, recvline, MAXLINE) == 0)
            perror("str_cli:server terminated prematurely");

        fputs(recvline, stdout);
    }
}

int main(int argc, char const* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2)
        perror("Usage: tcpcli <IPaddress>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8888);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);

    return 0;
}
