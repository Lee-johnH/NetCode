/****************************************************
 *
 * 演示epoll接收海量socket并进行处理响应的方法
 *
 ****************************************************/


#include "epoll.h"

#define MAXSIZE 1024
#define MAXEPOLLSIZE 10000

//设置句柄为非阻塞方式

int setnonblocking(int sockfd)
{
    if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
    {
        return -1;
    }

    return 0;
}

int main(int argc, char const* argv[])
{
    int listenfd, connfd, epfd, sockfd;
    int nfds, curfds, n;
    socklen_t len;

    struct sockaddr_in server_addr, client_addr;
    unsigned int myport, lisnum;
    char buffer[MAXSIZE + 1];

    //声明epoll_event结构体的变量,ev用于注册事件,events数组用于回传要处理的事件
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];


    if(argc != 2)
    {
        fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
        exit(1);
    }

    if((myport = atoi(argv[1])) < 0)
    {
        fprintf(stderr, "Usage:%s portnumber\a\n", argv[0]);
        exit(1);
    }

    //开启socket监听
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket error:%s \a\n", strerror(errno));
        exit(1);
    }

    //把socket设置为非阻塞方式
    setnonblocking(listenfd);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(myport);

    if(bind(listenfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Bind error:%s\a\n", strerror(errno));
        exit(1);
    }

    if(listen(listenfd, 5) == -1)
    {
        fprintf(stderr, "Listen error:%s\a\n", strerror(errno));
        exit(1);
    }

    //创建epoll句柄,把监听socket加入到epoll集合里
    epfd = epoll_create(MAXEPOLLSIZE);      //epoll专用的文件描述符
    //len = sizeof(struct sockaddr_in);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;

    //将listenfd注册到epoll事件
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) < 0)
    {
        fprintf(stderr, "epoll set insertion error: fd = %d\n", listenfd);
        return -1;
    }

    curfds = 1;

    while(1)
    {
         //等待有事件发生
         nfds = epoll_wait(epfd, events, curfds, -1);

         if(nfds == -1)
         {
              perror("epoll_wait");
              break;
         }

         //处理所有事件
         for(n = 0; n < nfds; ++n)
         {
              //如果新检测到一个SOCKET用户连接到了绑定的SOCKET端口,建立新连接
              if(events[n].data.fd == listenfd)
              {
                  len = sizeof(struct sockaddr_in);
                  if((connfd = accept(listenfd, (struct sockaddr *)(&client_addr), &len)) < 0)
                  {
                       perror("accept error");
                       continue;
                  }
                  else {
                      printf("有新链接来自于: %s:%d \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                  }

                  setnonblocking(connfd);

                  //设置用于注册的读操作事件
                  ev.events = EPOLLIN | EPOLLET;
                  //设置用于读操作的文件描述符
                  ev.data.fd = connfd;

                  //注册ev
                  epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
                  curfds++;
              }
              else if(events[n].events & EPOLLIN)   //如果是已连接的用户,并且收到数据,那么进行读入
              {

                  printf("EPOLLIN\n");
                  if((sockfd = events[n].data.fd) < 0)
                      continue;
                  int len;
                  bzero(buffer, MAXSIZE + 1);

                  //接收客户端的消息
                  if((len = recv(sockfd, buffer, MAXSIZE, 0)) > 0)
                  {
                       printf("接收消息成功:'%s', 共%d个字节的数据\n", buffer, len);
                  }
                  else
                  {
                      if(len < 0)
                      {
                           printf("接收消息失败!错误代码是%d,错误信息是'%s'\n", errno, strerror(errno));
                           epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
                           curfds--;
                           continue;
                      }
                  }

                  //设置用于写操作的文件描述符
                  ev.data.fd = sockfd;
                  //设置用于注册的写操作事件
                  ev.events = EPOLLOUT | EPOLLET;

                  //修改sockfd上要处理的事件为EPOLLOUT
                  epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev); //修改标识符,等待下一个循环时发送数据,异步处理的精髓!!!
              }
              else if(events[n].events & EPOLLOUT) //如果有数据发送
              {
                  printf("EPOLLOUT\n");
                  sockfd = events[n].data.fd;

                  bzero(buffer, MAXSIZE + 1);
                  strcpy(buffer, "Server already processes!");

                  send(sockfd, buffer, strlen(buffer), 0);

                  //设置用于读操作的文件描述符
                  ev.data.fd = sockfd;
                  //设置用于注册的读操作
                  ev.events = EPOLLIN | EPOLLET;

                  //修改sockfd上要处理的事件为EPOLLIN
                  epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
              }
         }
    }

    close(listenfd);

    return 0;
}

