#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAXSIZE 1024

typedef struct
{
    long type;
    char buff[MAXSIZE];
}MSG;

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("usage: %s key type\n", argv[0]);
        exit(1);
    }

    /*key_t key = ftok(argv[1], 0);*/
    key_t key = atoi(argv[1]);
    long type = atoi(argv[2]);

    //获得指定的消息队列
    int msq_id;
    if((msq_id = msgget(key, 0777)) < 0)
    {
        perror("msgget error!");
        exit(1);
    }
    printf("msq id: %d\n", msq_id);

    //从消息队列中接受指定类型的消息
    MSG m;
    if(msgrcv(msq_id, &m, sizeof(MSG)-sizeof(long), type, IPC_NOWAIT) < 0)
    {
        perror("msgrcv error!");
    }
    else
    {
        printf("type: %ld\nmessage: %s\n", m.type, m.buff);
    }

    return 0;
}
