/**********************************************************************************
* 信号捕捉设定：
* #include <signal.h>
*   int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
*   stuct sigaction{
*       void (*sa_handler)(int);
*       void (*sa_sigaction)(int, siginfo_t *, void *);
*       sigset_t sa_mask;
*       int sa_flags;
*       void (*sa_restorer)(void);
*   }
*
*
***********************************************************************************/
