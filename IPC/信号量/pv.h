#ifndef _PV_H_
#define _PV_H_

//初始化semnums个信号灯的值(value)
extern int I(int semnums, int value);

//对信号量集(semid)中的信号灯(semnum)作P(value)操作
extern void P(int semid, int semnum, int value);

//对信号量集(semid)中的信号灯(semnum)作V(value)操作
extern void V(int semid, int semnum, int value);

//销毁信号量集(semid)
extern void D(int semid);


#endif
