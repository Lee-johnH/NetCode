#include <stdio.h>

int main(int argc, char const* argv[])
{
    union {
        short value;
        char test[2];
    }endian;

    endian.value = 0x0102;

    if(endian.test[0] == 1 && endian.test[1] == 2)
    {
        printf("big endian....\n");
    }
    else if(endian.test[0] == 2 && endian.test[1] == 1)
    {
        printf("little endian....\n");
    }
    else
        printf("unknow....\n");

    return 0;
}
