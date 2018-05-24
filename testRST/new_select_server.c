#include "select.h"

#define MAXLINE 80

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

void printset(const fd_set *set)
{
    int i;

    for(i = 0; i < 32; i++)
        if(FD_ISSET(i, set) == 1)
            putchar('1');
        else
            putchar('0');
    printf("\n");
}

int main(int argc, char const* argv[])
{
    int i, maxi, maxfd, listenfd, connfd, sockfd;
    int portnumber;
    int nready, client[FD_SETSIZE];     /*FD_SETSIZE默认为1024*/
    ssize_t messlen;
    fd_set rset, allset;                /*rset表示有数据到来的文件描述符,allset表示要监听的所有描述符*/
    char buf[MAXLINE];
    socklen_t cliaddr_len;
    struct sockaddr_in cli_addr, server_addr;


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
        fprintf(stderr, "Socket error:%s \a\n", strerror(errno));
        exit(1);
    }

    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portnumber);

    if(bind(listenfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Bind error:%s\a\n", strerror(errno));
        exit(1);
    }

    /*监听listenfd描述符*/
    if(listen(listenfd, 20) == -1)
    {
        fprintf(stderr, "Listen error:%s\a\n", strerror(errno));
        exit(1);
    }

    maxfd = listenfd;   /*初始化*/
    maxi = -1;          /*client[]的下标*/

    for(i=0; i<FD_SETSIZE; i++)
        client[i] = -1;     /*用-1初始化client[]*/

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);      /*构造select监控文件描述符集合*/

    for( ; ; )
    {
        rset = allset;         /*每次循环时都重新设置select监控信号集*/

        printf("before select: ");
        printset(&rset);

        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        printf("after  select: ");
        printset(&rset);
        printf("select return!\n");

        if(nready < 0)
        {
            perror("select error!");
            exit(1);
        }

        if(FD_ISSET(listenfd, &rset))  /*有新的客户连接请求到来*/
        {
            cliaddr_len = sizeof(cli_addr);
            if((connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &cliaddr_len)) == -1)
            {
                perror("Accept error!");
            }

            printf("Server get connection from %s\n", inet_ntoa(cli_addr.sin_addr));

            for(i = 0; i < FD_SETSIZE; i++)
                if(client[i] < 0)
                {
                    client[i] = connfd;      /*保存accept返回的文件描述符到client[]里*/
                    break;
                }

            /*达到select能监控的文件个数上限 1024*/
            if(i == FD_SETSIZE)
            {
                fputs("Too many clients!", stderr);
                exit(1);
            }

            FD_SET(connfd, &allset);      /*添加一个新的文件描述符到监控信号集里*/

            if(connfd > maxfd)
                maxfd = connfd;       /*select第一个参数需要*/
            if(i > maxi)
                maxi = i;             /*更新client[]最大下标值*/

            if(--nready == 0)
                continue;             //如果没有更多的就绪描述符继续回到上面的select阻塞监听,负责处理未处理完的就绪文件描述符
        }

        for(i=0; i <= maxi; i++){      /*检测哪个client有数据就绪*/
            if((sockfd = client[i]) < 0)
                continue;

            if(FD_ISSET(sockfd, &rset))
            {
                bzero(buf, MAXLINE);
                if((messlen = recv(sockfd, buf, MAXLINE, 0)) > 0)
                {
                    printf("接收消息成功:'%s', 共%ld个字节的数据\n", buf, messlen);
                    send(sockfd, buf, strlen(buf), 0);
                }
                else if(messlen == 0)
                {
                    /*当client关闭连接时,服务器端也关闭对应的连接*/
                    printf("对方关闭连接，与该客户聊天结束!\n");
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
               }
                else{
                    printf("接收消息失败!错误代码是%d, 错误消息是'%s'\n", errno, strerror(errno));
                    /*close(sockfd);*/
                    /*FD_CLR(sockfd, &allset);*/
                    /*client[i] = -1;*/
                    /*exit(1);*/
                }

                if(--nready == 0)
                    break;
            }
        }
    }


    close(listenfd);


    return 0;
}
