#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024
#define true    1
#define false   0
typedef int bool;

typedef struct fds
{
    int epollfd;
    int sockfd;
}Fds;

int setnonblocking(int fd)
{
     int old_fd = fcntl(fd, F_GETFL);
     int new_fd = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

     return old_fd;
}

/*将fd上的EPOLLIN和EPOLLET事件注册到epollfd指示的epoll内核事件表中，参数oneshot指定是否注册fd上的EPOLLONESHOT事件*/
void addfd(int epollfd, int fd, bool oneshot)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if(oneshot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);

}

/*重置fd上的事件。这样操作之后，尽管fd上的EPOLLONESHOT事件被注册，但是操作系统仍然会触发fd上的EPOLLIN事件，且只触发一次*/
void reset_oneshot(int epollfd, int fd)
{
     struct epoll_event event;
     event.data.fd = fd;
     event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
     epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

/*工作线程*/
void *worker(void *arg)
{
     int sockfd = ((Fds *)arg)->sockfd;
     int epollfd = ((Fds *)arg)->epollfd;
     char buf[BUFFER_SIZE];
     memset(buf, '\0', BUFFER_SIZE);

     printf("start new thread to receive data on fd: %d\n", sockfd);

     /*循环读取sockfd上的数据，直到遇到EAGAIN错误*/
     while(1)
     {
          int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
          if(ret == 0)
          {
              close(sockfd);
              printf("client closed connection!\n");
              break;
          }
          else if (ret < 0)
          {
              if(errno == EAGAIN)
              {
                   reset_oneshot(epollfd, sockfd);
                   printf("read finished\n");
                   break;
              }
          }
          else
          {
               printf("Received data from client: %s\n", buf);
               /*休眠5s,模拟数据处理过程*/
               sleep(5);
          }
     }
     printf("end thread receiving data on fd: %d\n", sockfd);
}

int main(int argc, char *argv[])
{
     if(argc <= 2)
     {
         printf("usage: %s ip_address portnumber\n", argv[0]);
         exit(1);
     }
     int portnumber = atoi(argv[2]);
     int ret = 0;
     struct sockaddr_in address;

     bzero(&address, sizeof(address));
     address.sin_family = AF_INET;
     address.sin_addr.s_addr = htonl(INADDR_ANY);
     address.sin_port = htons(portnumber);

     int listenfd = socket(AF_INET, SOCK_STREAM, 0);
     assert(listenfd >= 0);

     ret = bind(listenfd, (struct sockaddr *)&address, sizeof(address));
     assert(ret != -1);

     ret = listen(listenfd, 5);
     assert(ret != -1);

     struct epoll_event events[MAX_EVENT_NUMBER];
     int epollfd = epoll_create(MAX_EVENT_NUMBER);
     assert(epollfd != -1);

     /*注意，listenfd是不能注册EPOLLONESHOT事件的，否则应用程序只能处理一个客户连接；因为后续的客户连接请求将不再触发listenfd上的EPOLLIN事件*/
     addfd(epollfd, listenfd, false);

     while(1)
     {
          int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
          if(ret < 0)
          {
              printf("epoll failure!\n");
              break;
          }

          for(int i = 0; i < ret; i++)
          {
               int sockfd = events[i].data.fd;

               if(sockfd == listenfd)
               {
                   struct sockaddr_in client_address;
                   socklen_t client_addrlen = sizeof(client_address);;
                   int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlen);
                   /*对每个非监听文件描述符都注册EPOLLONESHOT事件*/
                   addfd(epollfd, connfd, true);
               }
               else if(events[i].data.fd & EPOLLIN)
               {
                    pthread_t thread;
                    Fds fd_for_worker;
                    fd_for_worker.epollfd = epollfd;
                    fd_for_worker.sockfd = sockfd;
                    /*启用一个线程为sockfd服务*/
                    pthread_create(&thread, NULL, worker, (void *)&fd_for_worker);
               }
               else
               {
                    printf("something else happened\n");
               }
          }
     }
     close(listenfd);
     return 0;


}









