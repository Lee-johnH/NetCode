/************************服务器程序********************************/

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

void chat(int new_fd)
{
    int len;
    char buffer[MAXSIZE];
    //接收消息
    if((len = read(new_fd, buffer, MAXSIZE)) > 0)
    {
        buffer[len] = '\0';
        printf("客户端发来: %s\n", buffer);
    }
    else
    {
        printf("Read Error!\a\n");
        exit(1);
    }
    //发送消息
    printf("向对方发送的消息是: ");
    scanf("%s", buffer);
    if(write(new_fd, buffer, MAXSIZE) == -1)
    {
        printf("Write Error!\a\n");
        exit(1);
    }
}

int main(int argc, char const* argv[])
{
    int sockfd, new_fd;
    struct sockaddr_in server_addr;     //网络IPv4套接字地址
    struct sockaddr_in client_addr;
    int sin_size, portnumber;
    int childpid;

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

    /*服务器端创建socket描述符*/
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket error:%s\a\n", strerror(errno));
        exit(1);
    }

    /*服务器端填充sockaddr结构*/
    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portnumber);

    /*绑定sockfd描述符(绑定通用套接字地址)*/
    if(bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Bind error:%s\a\n", strerror(errno));
        exit(1);
    }

    /*监听sockfd描述符*/
    if(listen(sockfd, 5) == -1)
    {
        fprintf(stderr, "Listen error:%s\a\n", strerror(errno));
        exit(1);
    }

    while(1)
    {
        /*服务器阻塞,直至客户端建立连接*/
        sin_size = sizeof(struct sockaddr_in);
        if((new_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) == -1)
        {
            fprintf(stderr, "Accept error:%s\a\n", strerror(errno));
            exit(1);
        }

        fprintf(stderr, "Server get connection from %s\n", inet_ntoa(client_addr.sin_addr));

        if((childpid = fork()) == 0)    //为每个客户创建一个进程
        {
            close(sockfd);
            for(;;)
                chat(new_fd);
            exit(0);

        }
        //通讯结束
        //close(new_fd);
        //循环下一个
    }

    close(sockfd);
    exit(0);

    return 0;
}
