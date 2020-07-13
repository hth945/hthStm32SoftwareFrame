#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include "stm32f4xx.h"


//设置 SYSTICK时钟频率 ，TICK的中断频率
#define SYSTICK_FRE_HZ 1000  //1000 = 1ms


/*---------多任蛰rg定x--------------------*/
#define MAX_TASK 	 1          //定义任务数量
extern unsigned int task_time_ms[MAX_TASK];  

/*-----------------------------------------------*/

//rg苏I
extern unsigned int _1ms_ok;
extern unsigned int _20ms_ok;

//初始化TICK
void SysTick_Init(void);


//y量rg的函
//单位1ms
unsigned int MeasureTimeStart_ms(void);
unsigned int GetMeasureTime_ms(unsigned int ts);


#endif

/********************************* SKYCODE ************************************/
