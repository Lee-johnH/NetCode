#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include "wrap.h"

#define MAXLINE 80

int main(int argc, char const* argv[])
{
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    int sockfd, portnumber, n;
    struct hostent *host;

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

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portnumber);
    servaddr.sin_addr = *((struct in_addr *)host->h_addr);

    Connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    while(fgets(buf, MAXLINE, stdin) != NULL)
    {
         Write(sockfd, buf, strlen(buf));
         n = Read(sockfd, buf, MAXLINE);
         if(n == 0)
             printf("the other side has been closed.\n");
         else
             Write(STDOUT_FILENO, buf, n);
    }

    Close(sockfd);
    return 0;
}
