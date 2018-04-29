/*该程序阻塞某个信号然后观察*/
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void printsigset(const sigset_t *set)
{
    int i;

    for(i = 1; i < 32; i++)
        if(sigismember(set, i) == 1)
            putchar('1');
        else
            putchar('0');
    printf("\n");
}

#if 0
//测试时,启动程序,在终端按下<ctrl+c>分析代码
int main(int argc, char const* argv[])
{
    sigset_t s, p;      //printf("%d\n", sizeof(s));    //128Bytes

    sigemptyset(&s);
    sigaddset(&s, SIGINT);
    sigprocmask(SIG_BLOCK, &s, NULL);

    while(1)
    {
         sigpending(&p);
         printsigset(&p);
         sleep(1);
    }

    return 0;
}
#endif

#if 1
int main(int argc, char const* argv[])
{
    int i = 0;
    sigset_t s, p;

    sigemptyset(&s);
    sigaddset(&s, SIGINT);
    sigaddset(&s, SIGQUIT);
    sigprocmask(SIG_BLOCK, &s, NULL);
    while(1)
    {
         sigpending(&p);    //读取当前进程的未决信号集，通过p传出
         printsigset(&p);
         if(i == 10)
         {
             sigdelset(&s, SIGQUIT);                //SIGQUIT=0 SIGINT=1
             sigprocmask(SIG_UNBLOCK, &s, NULL);    //mask &= ~s
         }
         sleep(1);
         i++;
    }

    return 0;
}
#endif
