#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include "wrap.h"
#include <ctype.h>
#include <fcntl.h>

#define MAXLINE 10
#define OPEN_MAX 1024
#define true    1
#define false   0

int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);

    return old_option;
}

/*将文件描述符fd上的EPOLLIN注册到epollfd指示的epoll内核实践表中，参数enable_et指定是否对fd启用ET模式*/
void addfd(int epollfd, int fd, int enable_et)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(enable_et)
    {
        event.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

/*LT模式的工作流程*/
void LT(struct epoll_event* events, int number, int epollfd, int listenfd)
{
    char buf[MAXLINE];
    for(int i = 0; i < number; i++)
    {
        if(!(events[i].events & EPOLLIN))   /*假如该描述符无数据可读，则跳过*/
            continue;

        int sockfd = events[i].data.fd;
        if(sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
            addfd(epollfd, connfd, false);    /*对connfd禁用ET模式*/
        }
        else if(events[i].events & EPOLLIN)
        {
            /*只要socket读缓存中还有未读出的数据，这段代码就被触发*/
            printf("event trigger once\n");
            memset(buf, '\0', MAXLINE);
            int ret = recv(sockfd, buf, MAXLINE-1, 0);
            if(ret <= 0)
            {
                printf("received error!\n");
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content: %s\n", ret, buf);
        }
        else {
            printf("something else happened \n");
        }
    }
}

/*ET模式的工作流程*/
void ET(struct epoll_event* events, int number, int epollfd, int listenfd)
{
    char buf[MAXLINE];
    for(int i = 0; i < number; i++)
    {
        int sockfd = events[i].data.fd;
        if(sockfd == listenfd)
        {
            struct sockaddr_in client_address;
            socklen_t client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
            addfd(epollfd, connfd, true);    /*对connfd开启ET模式*/
        }
        else if(events[i].events & EPOLLIN)
        {
            /*这段代码不会被重复触发，所以我们循环读取数据，以确保把socket读缓冲区中的所有数据读出*/
            printf("event trigger once\n");
            while(1)
            {
                memset(buf, '\0', MAXLINE);
                int ret = recv(sockfd, buf, MAXLINE-1, 0);
                if(ret < 0)
                {
                    /*对于非阻塞IO,下面的条件成立表示数据已经全部读取完毕。此后，epoll就能再次触发sockfd上的EPOLLIN事件，以驱动下一次读操作*/
                    if((errno == EAGAIN) || (errno == EWOULDBLOCK))
                    {
                        printf("read later\n");
                        break;
                    }
                    printf("client closed connection! received error!\n");
                    close(sockfd);
                    break;
                }
                else if(ret == 0)
                {
                    close(sockfd);
                }
                else
                {
                    printf("get %d bytes of content: %s\n", ret, buf);
                }
            }
        }
        else {
            printf("something else happened \n");
        }
    }
}

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

    if((efd = epoll_create(OPEN_MAX)) == -1)
        perr_exit("epoll_create");

    addfd(efd, listenfd, false);

    while(1)
    {
        int ret = epoll_wait(efd, ep, OPEN_MAX, -1);
        if(ret < 0)
        {
            printf("epoll failure\n");
            break;
        }

        //LT(ep, ret, efd, listenfd);    /*使用LT模式*/
        ET(ep, ret, efd, listenfd);  /*使用ET模式*/

    }

    close(listenfd);
    close(efd);

    return 0;
}
