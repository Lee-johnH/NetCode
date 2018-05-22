#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>

#define MAXSIZE 1024

typedef struct
{
    long type;          //消息类型
    char buff[MAXSIZE];    //消息数据本身
}MSG;

/*
 * 往消息队列中发送消息
 *
 */

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("usage: %s key\n", argv[0]);
        exit(1);
    }

    /*key_t key = IPC_PRIVATE;*/
    /*key_t key = ftok(argv[0], 0);*/
    key_t key = atoi(argv[1]);
    printf("ket: %d\n", key);

    //创建消息队列
    int msg_id;
    if((msg_id = msgget(key, IPC_CREAT | IPC_EXCL | 0777)) < 0)
    {
        perror("msgget error!");
        exit(1);
    }
    printf("msq id: %d\n", msg_id);

    //定义要发送的消息
    MSG m1 = {1, "123"};
    MSG m2 = {2, "234"};
    MSG m3 = {3, "345"};
    MSG m4 = {4, "456"};
    MSG m5 = {4, "1234567"};

    //发送消息到消息队列
    if(msgsnd(msg_id, &m1, sizeof(MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {   //注意：此处的消息长度为消息本身的长度(不包括消息类型的长度)
        perror("msgsnd error!");
    }
    if(msgsnd(msg_id, &m2, sizeof(MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {
        perror("msgsnd error!");
    }
    if(msgsnd(msg_id, &m3, sizeof(MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {
        perror("msgsnd error!");
    }
    if(msgsnd(msg_id, &m4, sizeof(MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {
        perror("msgsnd error!");
    }
    if(msgsnd(msg_id, &m5, sizeof(MSG) - sizeof(long), IPC_NOWAIT) < 0)
    {
        perror("msgsnd error!");
    }

    //发送消息后去获得消息队列中消息的总数
    struct msqid_ds ds;
    if(msgctl(msg_id, IPC_STAT, &ds) < 0)
    {
        perror("msgctl error!");
    }
    printf("msg total: %ld\n", ds.msg_qnum);

    return 0;
}
