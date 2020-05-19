/********************************* SKYCODE *************************************
* Copyright (c) 2016-2017, 深圳思凯测试技术有限公司（V州天a子技g有限公司）
* All rights reserved.
*
* 文件名   ：delay.c 
* 描述     : 延时程序
*            
*
* 版    本 ：V0.4
* 作者     ：SKYCODE2013
* 完成日期 ：2017-3-30
* 修改描述 ：1. 修改Delay_us()函数的漏洞 使之精确
*
* 版    本 ：V0.1
* 作者     ：SKYCODE2013
* 完成日期 ：2016
* 修改描述 ：1. Delay_us()来源RT Thread
*							                          	
*******************************************************************************/

#include "delay.h"
#include "systick.h"

/********************************************************************************
*函得：Delay_us
*描述  ：微秒延时函数，不能超过1000us
*参数  ：us - 延时时间 单位us
*返回  ：o
********************************************************************************/
void Delay_us(u32 us) 
{ 
    u32 delta; 
    u32 current_delay; 
    u32 tick_val;
    u32 us2;

    // 获得延时经过的tick数
    us = (us * SysTick->LOAD)/(1000000/SYSTICK_FRE_HZ); //2017-2-14 skycode

    while(us)  //2017-3-30 skycode 
    {
        // 获得当前时间  
        delta = SysTick->VAL;
        
        if(us>500){us2=500; us-=500;} //2017-3-30 skycode 
        else {us2=us; us=0;}
         

        // 循环获得当前时间，直到达到指定的时间后退出循环
        do 
        { 
            tick_val = SysTick->VAL;  //2017-3-30 skycode 
            if (delta > tick_val)
            {
                current_delay = delta - tick_val; 
            }
            else
            {
                // 延时跨越了一次OS tick的边界时的处理
                current_delay = SysTick->LOAD + delta - tick_val; 
            }
        } while(current_delay < us2); 
    }
}

/********************************************************************************
*函得：Delay_ms
*描述  ：毫秒延时程序
*参数  ：ms - 延时时间 单位ms
*返回  ：o
********************************************************************************/
void Delay_ms(u32 ms)
{
    while(ms--)
    {
        Delay_us(1000);
    }
}

/********************************* SKYCODE ************************************/
