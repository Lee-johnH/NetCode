#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

int main(void)
{
    FILE *fp;
    //命令执行的结果放在fp指向的结构体缓存中
    fp = popen("cat /etc/passwd", "r");

    char buf[512];
    memset(buf, 0, sizeof(buf));
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        printf("%s\n", buf);
    }

    pclose(fp);

    printf("--------------------------------------------\n");
    //为wc命令提供统计数据

    fp = popen("wc -l", "w");
    fprintf(fp, "1\n2\n3\n4\n");
    pclose(fp);

    return 0;
}
