/********************************* SKYCODE *************************************
* Copyright (c) 2014-2016, 深圳思凯测试技术有限公司（V州天a子技g有限公司）
* All rights reserved.
*
* 文件名   ：systick.c 
* 描述     : TICK时钟程序
*
* 版    本 ：V03
* 作者     ：SKYCODE2013
* 完成日期 ：2016-11-12
* 修改描述 ：1. SYYTICK 1ms, 可修改SYSTICK_FRE_HZ来改变
*
* 版    本 ：V02
* 作者     ：SKYCODE2013
* 完成日期 ：2014-09-11
* 修改描述 ：1.增加y量rg的函
*
* 版    本 ：V01
* 作者     ：SKYCODE2013
* 完成日期 ：2013
* 修改描述 ：
*							                          	
*******************************************************************************/

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
