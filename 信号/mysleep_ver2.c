/*****************************************************************************
 * 改进版本
 ******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void sig_alrm(int signo)
{
    /*nothing to do*/
}

unsigned int mysleep(unsigned int nsecs)
{
    struct sigaction newact, oldact;
    sigset_t newmask, oldmask, suspmask;
    unsigned int unslept;

    /*为SIGALRM信号设置捕捉函数,并保留之前的信号集以便恢复原来的捕捉函数*/
    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);   //在这里用到oldact是因为需要恢复原来的捕捉函数

    /*block SIGALRM and save current signal mask*/
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    alarm(nsecs);

    suspmask = oldmask;
    sigdelset(&suspmask, SIGALRM);       //确保SIGALRM信号不会被阻塞
    sigsuspend(&suspmask);               //将进程挂起并设置新的信号屏蔽字,变成原子操作,就不会有BUG了

    unslept = alarm(0);                 //这一步是为了返回未定时够的秒数
    sigaction(SIGALRM, &oldact, NULL);  //恢复原来的信号捕捉函数

    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    return unslept;
}

int main(int argc, char const* argv[])
{
    while(1)
    {
         mysleep(2);
         printf("Two seconds passed\n");
    }

    return 0;
}
