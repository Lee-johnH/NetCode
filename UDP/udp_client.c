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
    int sockfd, n;
    char buf[MAXLINE];
    socklen_t servaddr_len;
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

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(portnumber);
    servaddr.sin_addr = *((struct in_addr *)host->h_addr);

    while(fgets(buf, MAXLINE, stdin) != NULL)
    {
         n = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
         if(n == -1)
             perr_exit("sendto error");

         n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, 0);
         if(n == -1)
             perr_exit("recvfrom error");

         Write(STDOUT_FILENO, buf, n);
    }

    Close(sockfd);
    return 0;
}
