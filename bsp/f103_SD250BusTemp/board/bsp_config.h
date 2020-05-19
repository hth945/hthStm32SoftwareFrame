#ifndef _BSP_CONFIG_H_
#define _BSP_CONFIG_H_

#include "stdint.h"

/**********需要实现*******************/
//获取当前系统时间 ms 数
int P_getNowTime_ms(void);

//ms延时
int P_delay_ms(int ms);

//us延时
int P_delay_us(int us);


#endif
