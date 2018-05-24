#include "account.h"
#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "pv.h"

//取款
double withdraw(Account *a, double amt)
{
    assert(a != NULL);

    //对信号量集semid中的0号信号量/信号灯作P(1)操作
    P(a->semid, 0, 1);

    if(amt < 0 || amt > a->balance)
    {
        //对信号量集semid中的0号信号量/信号灯作V(1)操作
        V(a->semid, 0, 1);
        return 0.0;
    }

    double balance = a->balance;
    sleep(1);
    balance -= amt;
    a->balance = balance;

    //对信号量集semid中的0号信号量/信号灯作V(1)操作
    V(a->semid, 0, 1);

    return amt;
}

//存款
double deposit(Account *a, double amt)
{
    assert(a != NULL);

    //对信号量集semid中的0号信号量/信号灯作P(1)操作
    P(a->semid, 0, 1);

    if(amt < 0)
    {
        //对信号量集semid中的0号信号量/信号灯作V(1)操作
        V(a->semid, 0, 1);
        return 0.0;
    }

    double balance = a->balance;
    sleep(1);
    balance += amt;
    a->balance = balance;

    V(a->semid, 0, 1);

    return amt;
}

//查看账户余额
double get_balance(Account *a)
{
    assert(a != NULL);

    P(a->semid, 0, 1);
    double balance = a->balance;
    V(a->semid, 0, 1);

    return balance;
}
