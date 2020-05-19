#include "bsp_config.h"
#include "SysTick.h"
#include "delay.h"

int P_getNowTime_ms(void)
{
	return MeasureTimeStart_ms();
}

//ms延时
int P_delay_ms(int ms)
{
	Delay_ms(ms);
	return 0;
}

//us延时
int P_delay_us(int us)
{
	Delay_us(us);
	return 0;
}




