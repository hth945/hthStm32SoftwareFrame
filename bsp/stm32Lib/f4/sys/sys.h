
#ifndef _SYS_H_
#define _SYS_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "systick.h"
#include "delay.h"


/* Exported functions ------------------------------------------------------- */

/*
*函得：Sys_CreateTask
*描述  ：添加需要定r绦械暮
*参数  ：task_num - 任站
         fp       - 函抵羔
         td       - g隔rg，即每隔tdrg 绦幸淮fp函
*返回  ：{用MeasureTimeStart()r到F在rgL度 单位1个SYSTICK (1ms)
*/
void Sys_CreateTask(u8 task_num, void (*fp)(void), unsigned int td);

#endif
