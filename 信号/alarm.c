/**************************************************************************
 * 学会如何使用alarm定时器；
 * 某种软条件已发生:定时器alarm到时，每个进程只有一个定时器
 *
 *************************************************************************/
#include <unistd.h>
#include <stdio.h>

int main(int argc, char const* argv[])
{
    int counter;
    alarm(1);
    for(counter = 0; 1; counter++)
        printf("counter=%d ", counter);
    printf("\n");

    return 0;
}
