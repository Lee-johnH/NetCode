#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>
#include "wrap.h"
#include <ctype.h>

#define MAXLINE 80
#define OPEN_MAX 1024

int main(int argc, char const* argv[])
{
    int i, j, maxi, listenfd, connfd, sockfd;
    int nready, portnumber;;
    ssize_t n;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cliaddr, servaddr;

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

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;  /*listenfd监听普通读事件*/

    for(i = 1; i < OPEN_MAX; i++)
        client[i].fd = -1;          /*用-1初始化client[]里剩下元素*/
    maxi = 0;                       /*client[]数组有效元素最大元素下标*/

    for(; ;)
    {
        nready = poll(client, maxi+1, -1);      /*阻塞*/

            if(client[0].revents & POLLRDNORM)     /*有客户端连接请求*/
            {
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

                printf("received from %s at PORT %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

                for(i = 1; i < OPEN_MAX; i++)
                {
                    if(client[i].fd < 0)
                    {
                        client[i].fd = connfd;    /*找到client[]中空闲的位置，存放accept返回的connfd*/
                        break;
                    }
                }

                if(i == OPEN_MAX)
                    perr_exit("too many clients");

                client[i].events = POLLRDNORM;      /*设置刚刚返回的connfd,监控读事件*/
                if(i > maxi)
                    maxi = i;           /*更新client数组最大下标*/

                if(--nready <= 0)
                    continue;           /*没有更多就绪事件，继续回到poll阻塞*/
            }

        for(i = 1; i <= maxi; i++)     /*处理client[]中有数据到来的client*/
        {
            if((sockfd = client[i].fd) < 0)
                continue;

            if(client[i].revents & (POLLRDNORM | POLLERR)){
                if((n = Read(sockfd, buf, MAXLINE)) < 0)
                {
                    if(errno == ECONNRESET){  /*当收到RST标志时*/
                        /*connection reset by client*/
                        printf("client[%d] aborted connection\n", i);
                        Close(sockfd);
                        client[i].fd = -1;
                    }
                    else
                        perr_exit("read error");
                }
                else if(n == 0)
                {
                     /*connection closed by client*/
                    printf("client[%d] closed connection\n", i);
                    Close(sockfd);
                    client[i].fd = -1;
                }
                else{
                     for(j = 0; j < n; j++)
                         buf[j] = toupper(buf[j]);
                     Writen(sockfd, buf, n);
                }

                if(--nready <= 0)
                    break;
            }
        }
    }




    return 0;
}
