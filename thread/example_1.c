/**/
#include "thread.h"

pthread_t ntid;

void printids(const char *s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();
    printf("%s pid %u (0x%x)\n", s, (unsigned int)pid, (unsigned int)tid);
}

void *thr_fn(void *arg)
{
    printids(arg);

    return NULL;
}

int main(int argc, char const* argv[])
{
    int err;

    err = pthread_create(&ntid, NULL, thr_fn, "new thread: ");
    if(err != 0)
    {
         fprintf(stderr, "can't create thread: %s\n", strerror(err));
         exit(1);
    }
    printids("main thread:");
    sleep(1);

    return 0;
}
