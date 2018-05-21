#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("usage:%s fifo\n", argv[0]);
        exit(1);
    }
    printf("open fifo write...\n");
    //打开命名管道
    int fd = open(argv[1], O_WRONLY);

    if(fd < 0)
    {
        printf("open fifo error!\n");
        exit(1);
    }
    else
    {
        printf("open fifo success: %d\n", fd);
    }

    char *s = "1234567890";
    size_t size = strlen(s);

    if(write(fd, s, size) < 0)
    {
        perror("write error!");
    }
    close(fd);

    return 0;
}
