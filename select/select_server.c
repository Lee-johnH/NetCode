#include "select.h"

#define MAXSIZE 1024

/******************************************************************************************************************************
 *
 *  int select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);
 *
 *  返回: 若有就绪描述符则为其数目,若超时则为0, 若出错则为-1
 *  参数: timeout: 告知内核等待所指定描述符中的任何一个就绪可花多长时间.
 *        readset, writeset, exceptset: 指定我们要让内核测试读,写,异常条件的描述符.
 *        maxfd: 指定待测试的描述符个数.
 *
 *
 ******************************************************************************************************************************/


int main(int argc, char const* argv[])
{
    int sockfd, new_fd;
    struct sockaddr_in server_addr, cli_addr;

    unsigned int portnumber;
    int sin_size, messlen;      //sin_size是sockaddr结构体的大小, messlen是发送消息的字节大小
    char buffer[MAXSIZE];

    fd_set rfds;            //描述符集.
    struct timeval tv;

    int retval, maxfd = -1;

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

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Socket error:%s \a\n", strerror(errno));
        exit(1);
    }

    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portnumber);

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
        printf("\n----------------等待新的连接到来-------------------\n");

        sin_size = sizeof(struct sockaddr);

        if((new_fd = accept(sockfd, (struct sockaddr *)(&cli_addr), &sin_size)) == -1)
        {
            fprintf(stderr, "Accept error:%s\a\n", strerror(errno));
            exit(1);
        }

        fprintf(stderr, "Server get connection from %s\n", inet_ntoa(cli_addr.sin_addr));

        //开始处理每个连接上的数据收发
        printf("\n准备就绪,可以开始聊天了....\n");

        while(1)
        {
            //把集合都清空
            FD_ZERO(&rfds);

            //把标准输入(stdin)句柄加入到集合中
            FD_SET(0, &rfds);

            //把当前连接(socket)句柄new_fd加入到集合中
            FD_SET(new_fd, &rfds);

            maxfd = 0;

            if(new_fd > maxfd)
            {
                maxfd = new_fd;
            }

            //设置最大等待时间
            tv.tv_sec = 5;
            tv.tv_usec = 0;

            //开始等待
            retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);

            if(retval == -1)
            {
                printf("将退出,select出错! %s", strerror(errno));
                break;
            }
            else if(retval == 0)
            {
                continue;
            }
            else
            {
                //判断当前IO是否是stdin
                if(FD_ISSET(0, &rfds))    //读取用户输入的内容发出去
                {
                    bzero(buffer, MAXSIZE);
                    scanf("%s", buffer);

                    messlen = send(new_fd, buffer, strlen(buffer)-1, 0);

                    if(messlen > 0)
                        printf("消息: '%s'发送成功, 共发送了%d个字节!\n", buffer, messlen);
                    else
                    {
                        printf("消息'%s'发送失败!错误代码为%d, 错误信息是:'%s'\n", buffer, errno, strerror(errno));
                        break;
                    }
                }

                //判断当前IO是否来自socket
                if(FD_ISSET(new_fd, &rfds))   //当前连接的socket上有消息到来则接收对方发过来的消息并显示
                {
                    bzero(buffer, MAXSIZE);
                    //接收客户端消息
                    messlen = recv(new_fd, buffer, MAXSIZE, 0);
                    if(messlen > 0)
                        printf("接收消息成功:'%s', 共%d个字节的数据\n", buffer, messlen);
                    else
                    {
                        if(messlen < 0)
                            printf("接收消息失败!错误代码是%d, 错误消息是'%s'\n", errno, strerror(errno));
                        else
                            printf("对方退出了,聊天终止\n");
                        break;
                    }
                }
            }
        }
    }
    return 0;
}
