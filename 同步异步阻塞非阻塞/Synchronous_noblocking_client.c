/*
 * 同步非阻塞模式I/O
 * 在这个模式中，系统调用是以非阻塞的形式打开的，这意味着I/O操作不会立即完成，
 * 操作可能会返回一个错误代码，说明这个命令不能立马满足。
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXSIZE 1024

int main(int argc, char const* argv[])
{
    int sockfd;
    char buffer[MAXSIZE];
    struct sockaddr_in server_addr;
    struct hostent *host;
    /*
     *  struct hostent
     *  {
     *      char *h_name;       //正式主机名
     *      char **h_aliases;   //主机别名
     *      int h_addrtype;     //主机IP地址类型:IPv4-AF_INET
     *      int h_length;       //主机IP地址字节长度
     *      char **h_addr_list; //主机的IP地址列表
     *  }
     *
     */
    int portnumber, len;
    int flags;

    if(argc != 3)
    {
        fprintf(stderr, "Usage:%s hostname portnumber\a\n", argv[0]);
        exit(1);
    }

    if((host = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "Gethostname error\n");
        exit(1);
    }

    if((portnumber = atoi(argv[2])) < 0)
    {
        fprintf(stderr, "Usage:%s hostname portnumber\a\n", argv[0]);
        exit(1);
    }

    //客户端创建sockfd描述符
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
        exit(1);
    }

    //客户端程序填充服务端信息
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);


    /***************************************************************************************
     *                          同步阻塞模式
     ***************************************************************************************/

    //客户端发起连接请求
    if(connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
        exit(1);
    }

    /*****************************************************************************************
     * 这里的socket套接字非阻塞必须放到connect函数之后设置，因为在connect之前设置的话会出错，
     * 出错会设置errno值为：EINPROGRESS。这种错误发生在对非阻塞的socket调用connect，而连接又
     * 没有立即建立时。
     *****************************************************************************************/

    /*将socket套接字设置为非阻塞*/
    flags = fcntl(sockfd, F_GETFL, 0);
    if(fcntl(sockfd,F_SETFL, flags|O_NONBLOCK) < 0)
    {
         perror("fcntl error!");
         exit(1);
    }

    /* 同步非阻塞模式 */
    while(1){
        //发送消息
        printf("向对方发送的消息是: ");
        scanf("%s", buffer);
        if(write(sockfd, buffer, MAXSIZE) == -1)
        {
            sleep(1);
            printf("sleep!\n");
            continue;
        }

        printf("%s发送成功!\n", buffer);
        break;
    }

    /* 同步非阻塞模式 */
    while(1){
        //接收消息
        if((len = read(sockfd, buffer, MAXSIZE)) == -1)
        {
            if(errno == EWOULDBLOCK)
            {
                sleep(1);
                printf("sleep\n");
                continue;
            }
            else
            {
                printf("read Error!\n");
                exit(1);
            }
        }
        else{
            buffer[len] = '\0';
            printf("服务器发来: %s\n", buffer);
            break;
        }
    }

    //结束通讯
    close(sockfd);
    exit(0);

    return 0;
}
