/*****************************************************************************
 * 这个版本实现的mysleep有个bug.
 *
 * 信号捕捉设定：
 * #include <signal.h>
 *   int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
 *   stuct sigaction{
 *       void (*sa_handler)(int);   //早期的捕捉函数,与sa_sigaction互斥,不可同时使用,两个通过sa_flags选择采用哪种函数
 *       void (*sa_sigaction)(int, siginfo_t *, void *); //新添加的捕捉函数,可以传参
 *       sigset_t sa_mask;  //在执行捕捉函数时,设置阻塞其他信号,sa_mask|进程阻塞信号集,退出捕捉函数后,还原回原有的阻塞信号集
 *       int sa_flags;      //SA_SIGINFO或者0
 *       void (*sa_restorer)(void); //保留,已过时,没什么用了
 *   }
 *
 * int pause(void)
 *      使调用进程挂起,直到有信号递达,如果递达信号是忽略,则继续挂起
 *  int sigsuspend(const sigset_t *mask)
 *      1.以通过指定mask来临时解除对某个信号的屏蔽
 *      2.然后挂起等待
 *      3.当被信号唤醒sigsuspend返回时,进程的信号屏蔽字恢复为原来的值
 *
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
    unsigned int unslept;

    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);

    alarm(nsecs);
    pause();

    unslept = alarm(0);
    sigaction(SIGALRM, &oldact, NULL);

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
