/*
 * 在这个模式中,用户空间的应用程序执行一个系统调用，并阻塞，直到系统调用完成为止(数据传输完成或发生错误)
 *
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

#define MAXSIZE 1024

int main(int argc, char const* argv[])
{
    int sockfd;
    char buffer[MAXSIZE];
    struct sockaddr_in server_addr;
    struct hostent *host;
    int portnumber, len;

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

    //连接成功
    while(1){
        //发送消息
        printf("向对方发送的消息是: ");
        scanf("%s", buffer);
        if(write(sockfd, buffer, MAXSIZE) == -1)
        {
            printf("Write Error!\a\n");
            exit(1);
        }

        //接收消息
        if((len = read(sockfd, buffer, MAXSIZE)) > 0)
        {
            buffer[len] = '\0';
            printf("服务器发来: %s\n", buffer);
        }
        else if(len == 0)
            continue;
        else
        {
            printf("Read Error!\a\n");
            exit(1);
        }
    }

    //结束通讯
    close(sockfd);
    exit(0);

    return 0;
}
