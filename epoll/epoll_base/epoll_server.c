#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include "wrap.h"
#include <ctype.h>
#include <errno.h>

#define MAXLINE 80
#define OPEN_MAX 1024

int main(int argc, char const* argv[])
{
    int i, j, maxi, listenfd, connfd, sockfd;
    int nready, portnumber, efd, res;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    int client[OPEN_MAX];
    struct sockaddr_in cliaddr, servaddr;
    struct epoll_event tep, ep[OPEN_MAX];

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

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portnumber);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, 20);

    for(i = 0; i < OPEN_MAX; i++)
        client[i] = -1;          /*用-1初始化client[]里剩下元素*/
    maxi = -1;

    if((efd = epoll_create(OPEN_MAX)) == -1)
        perr_exit("epoll_create");

    tep.events = EPOLLIN;
    tep.data.fd = listenfd;

    if((res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep)) == -1)
        perr_exit("epoll_ctl");

    for(; ;)
    {
        if((nready = epoll_wait(efd, ep, OPEN_MAX, -1)) == -1)      /*阻塞*/
            perr_exit("epoll_wait");

        for(i = 0; i < nready; i++)
        {
            if(!(ep[i].events & EPOLLIN))   /*假如该描述符无数据可读，则跳过*/
                continue;
            if(ep[i].data.fd == listenfd)
            {
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
                printf("received from %s at PORT %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

                for(j = 0; j < OPEN_MAX; j++)
                    if(client[j] < 0)
                    {
                         client[j] = connfd;    /*保存描述符*/
                         break;
                    }

                if(j == OPEN_MAX)
                    perr_exit("too many clients");

                if(j > maxi)
                    maxi = j;

                tep.events = EPOLLIN;
                tep.data.fd = connfd;
                if((res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep)) == -1)
                {
                     perr_exit("epoll_ctl");
                }

            }
            else {
                 sockfd = ep[i].data.fd;
                 if((n = Read(sockfd, buf, MAXLINE)) == 0)
                 {
                     for(j = 0; j <= maxi; j++){
                         if(client[j] == sockfd)
                         {
                              client[j] = -1;
                              break;
                         }
                     }
                     res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
                     if(res == -1)
                         perr_exit("epoll_ctl");

                     Close(sockfd);
                     printf("client[%d] closed connection\n", j);
                 }
                 else {
                     for(j = 0; j < n; j++)
                         buf[j] = toupper(buf[j]);
                     Writen(sockfd, buf, n);
                 }
            }
        }

    }

    close(listenfd);
    close(efd);

    return 0;
}
