#include "select.h"

#define MAXSIZE 1024

int main(int argc, char const* argv[])
{
    int sockfd, len;
    struct sockaddr_in server_addr;
    char buffer[MAXSIZE];
    fd_set rfds;
    struct timeval tv;

    int retval, maxfd = -1;
    struct hostent *host;
    int portnumber;

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

    //创建socket用于tcp通信
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
        exit(1);
    }

    //客户端程序填充服务端信息
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnumber);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);

    //客户端发起连接请求
    if(connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
        exit(1);
    }

    printf("\n准备就绪,可以开始聊天了...\n");

    while(1)
    {
        //把集合清空
        FD_ZERO(&rfds);
        //把标准输入句柄0加入到集合中
        FD_SET(0, &rfds);

        maxfd = 0;
        //把当前连接句柄sockfd加入到集合中
        FD_SET(sockfd, &rfds);

        if(sockfd > maxfd)
            maxfd = sockfd;

        //设置最大等待时间
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        //开始等待
        retval = select(maxfd + 1, &rfds, NULL, NULL, &tv);

        if(retval == -1)
        {
            printf("将退出, select出错! %s", strerror(errno));
            break;
        }
        else if(retval == 0)
        {
            continue;
        }
        else
        {
            if(FD_ISSET(sockfd, &rfds))   //连接的socket上有消息到来则接受对方发过来的消息并显示
            {
                bzero(buffer, MAXSIZE);
                //接受对方发过来的消息, 最多接受MAXSIZE个字节
                len = recv(sockfd, buffer, MAXSIZE, 0);
                /*
                 * 关于send和recv函数要注意：
                 *  1.send函数中，第三个参数表明实际要发送的数据的字节数
                 *      例如：char buf[5];
                 *      send(sockfd, buf, 10, 0);
                 *      尽管buf长度为5个字节，但发送给对端的数据为&buf[0]开始之后10个字节，
                 *      即使这10个字节全部为'\0'。在接收端，recv函数就可以把这10个'\0'全部
                 *      接收到(当然，recv的buffer>=10)
                 *
                 *  2.这个时候，在另一端recv(sockfd, buf, len, 0)
                 *  buf能接收的长度最大为len。接收缓冲区中'\0'后面的数据也可以正常接收。
                 *
                 *  注意，在调用recv前，最好将buffer清空，recv函数不会在接收到的数据后面
                 *  添加结束字符'\0'.
                 *
                 */

                if(len > 0)
                {
                    buffer[len] = '\0';
                    printf("接收消息为: '%s', 共%d个字节的数据\n", buffer, len);
                }
                else
                {
                    if(len < 0)
                    {
                        printf("接收消息失败!错误代码是%d, 错误信息是'%s'\n", errno, strerror(errno));
                    }
                    else
                    {
                        printf("对方退出了,聊天终止!\n");
                    }

                    break;
                }
            }

            if(FD_ISSET(0, &rfds))    //读取标准输入的数据并发送
            {
                bzero(buffer, MAXSIZE);
                //fgets(buffer, MAXSIZE, stdin);
                scanf("%s", buffer);

                //发消息给服务器
                if((len = send(sockfd, buffer, strlen(buffer), 0)) == -1)
                {
                    printf("Send Error!\a\n");
                    exit(1);
                }
                else {
                        printf("消息: '%s'发送成功, 共发送了%d个字节!\n", buffer, len);
                }
            }
        }
    }


    //关闭连接
    close(sockfd);

    return 0;
}
