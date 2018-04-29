#include "thread.h"

struct student
{
    int age;
    char name[20];
    char id[4];
};

void *thread_fun(void *stu)
{
    sleep(1);
    printf("student age is %d, name is %s, id is %s\n", ((struct student *)stu)->age, ((struct student *)stu)->name, ((struct student *)stu)->id);
    return (void *)0;
}

int main(int argc, char const* argv[])
{
    pthread_t tid;
    int err;
    int *rval;

    struct student stu;
    stu.age = 20;
    memcpy(stu.name, "zhangsan", 20);
    memcpy(stu.id, "0007", 5);

    err = pthread_create(&tid, NULL, thread_fun, (void *)(&stu));
    if(err != 0)
    {
        printf("create new thread failed\n");
        return 0;
    }

    int i;
    printf("main thread have %d args\n", argc);
    for (i = 0; i < argc; i++) {
        printf("main thread args is %s\n", argv[i]);
    }

    //sleep(1);
    pthread_exit(rval);

    //return 0;
}
