/*****************************************************************************************************
 *  connect出错时有一种错误码: EINPROGRESS. 这个错误发生在对非阻塞的socket调用connect,而连接又没有
 * 立即建立时，在这种情况下，我们可以调用select、poll、epoll等函数来监听这个连接失败的socket上的可
 * 写事件。当select、poll等函数返回，再利用getsockopt来读取错误码并清除该socket上的错误。如果错误码
 * 是0，表示连接成功建立，否则连接失败。
 *
 *****************************************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1023

int setnonblocking(int fd)
{
     int old_opt = fcntl(fd, F_GETFL);
     int new_opt = old_opt | O_NONBLOCK;
     fcntl(fd, F_SETFL, new_opt);

     return old_opt;
}

/*超时连接函数，参数分别是服务器IP地址、端口号和超时时间(毫秒)。函数成功时返回已经处于连接状态的socket，失败则返回-1 */
int unblock_connect(const char *ip, int port, int time)
{
     int ret = 0;
     struct sockaddr_in address;

     bzero(&address, sizeof(address));
     address.sin_family = AF_INET;
     inet_pton(AF_INET, ip, &address.sin_addr);
     address.sin_port = htons(port);

     //int sockfd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
     int fdopt = setnonblocking(sockfd);

     if((ret = connect(sockfd, (struct sockaddr *)&address, sizeof(address))) == 0)
     {
          /*如果连接成功，则恢复sockfd的属性，并立即返回*/
         printf("connect with server immediately\n");
         fcntl(sockfd, F_SETFL, fdopt);

         return sockfd;
     }
     else if(errno != EINPROGRESS)
     {
          /*如果连接没有立即建立，而错误码又不是EINPROGRESS，就证明该链接已经出错了*/
         printf("unblock connect not support\n");

         return -1;
     }

     fd_set readfds;
     fd_set writefds;
     struct timeval timeout;

     FD_ZERO(&readfds);
     FD_SET(sockfd, &writefds);

     timeout.tv_sec = time;
     timeout.tv_usec = 0;

     if((ret = select(sockfd + 1, NULL, &writefds, NULL, &timeout)) <= 0)
     {
          /*select超时或者出错，立即返回*/
         printf("connection time out\n");
         close(sockfd);
         return -1;
     }

     if(!FD_ISSET(sockfd, &writefds))
     {
          printf("no events on sockfd found\n");
          close(sockfd);
          return -1;
     }

     int error = 0;
     socklen_t length = sizeof(error);

     /*调用getsockopt来获取并清除sockfd上的错误*/
     if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &length) < 0)
     {
          printf("get socket option failed\n");
          close(sockfd);
          return -1;
     }

     /*错误号不为0表示连接出错*/
     if(error != 0)
     {
          printf("connection failed after select with the error: %d \n", error);
          close(sockfd);
          return -1;
     }

     /*连接成功*/
     printf("connection ready after select with the socket: %d \n", sockfd);
     fcntl(sockfd, F_SETFL, fdopt);
     return sockfd;
}

int main(int argc, char const* argv[])
{
    if(argc <= 2)
    {
         printf("usage: %s ip_address port_number\n", argv[0]);
         return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = unblock_connect(ip, port, 10);
    if(sockfd < 0)
    {
         return 1;
    }

    close(sockfd);
    return 0;
}
