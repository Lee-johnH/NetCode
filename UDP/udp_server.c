#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "wrap.h"
#include <arpa/inet.h>
#include <ctype.h>

#define MAXLINE 80

int main(int argc, char const* argv[])
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int sockfd, portnumber;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int i, n;

    if(argc != 2)
    {
        fprintf(stderr, "Usage:%s portnumber\n", argv[0]);
        exit(1);
    }

    if((portnumber = atoi(argv[1])) < 0)
    {
        fprintf(stderr, "Usage:%s portnumber\n", argv[0]);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portnumber);

    Bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("Accepting connection...\n");

    while(1)
    {
         cliaddr_len = sizeof(cliaddr);
         n = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len);
         if(n == -1)
             perr_exit("recvfrom error");

         printf("received from %s at PORT %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

         for(i = 0; i < n; i++)
             buf[i] = toupper(buf[i]);
         n = sendto(sockfd, buf, n, 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
         if(n == -1)
             perr_exit("sendto error");
    }

    return 0;
}
