#include "sys.h"


#ifndef _DELAY_H_
#define _DELAY_H_

/*
*函得：Delay_us
*描述  ：微秒延时函数，不能超过1000us
*参数  ：us - 延时时间 单位us
*返回  ：o
*/
void Delay_us(u32 us);

/*
*函得：Delay_ms
*描述  ：毫秒延时程序
*参数  ：ms - 延时时间 单位ms
*返回  ：o
*/
void Delay_ms(u32 ms);

#endif

/********************************* SKYCODE ************************************/
