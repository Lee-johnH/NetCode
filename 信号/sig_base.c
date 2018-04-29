/***************************************************************************************************************
 * 学会使用几个基本的信号函数:
 *  信号集处理函数: sigset_t为信号集，可sizeof(sigset_t)查看
 *              int sigemptyset(sigset_t *set);
 *              int sigfillset(sigset_t *set);
 *              int sigaddset(sigset_t *set);
 *              int sigdelset(sigset_t *set);
 *              int sigismember(const sigset_t *set, int signo);
 *
 *  调用函数sigprocmask可以读取或更改进程的信号屏蔽字:
 *          #include <signal.h>
 *              int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
 *          返回值：若成功则为0,失败返回-1.
 *          参数: 如果oldset是非空指针，则它是一个传入传出参数。作用是备份。
 *                how参数: SIG_BLOCK   set包含了我们希望添加到当前信号屏蔽字的信号,相当于mask=mask|set
 *                         SIG_UNBLOCK set包含了我们希望从当前信号屏蔽字中解除阻塞的信号,相当于mask=mask&~set
 *                         SIG_SETMASK 设置当前信号屏蔽字为set所指向的值,相当于mask=set
 *
 *  sigspending读出当前进程的未决信号集，通过set参数传出。成功返回0，失败返回-1.
 *          #include <signal.h>
 *              int sigpending(sigset_t *set);
 ***************************************************************************************************************/
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
     puts("");
}

int main(int argc, char const* argv[])
{
    sigset_t s, p;
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
