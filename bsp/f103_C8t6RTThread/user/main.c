#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include <rtthread.h>
#include "mypwm.h"

stringCMD_t scmd;
u8 fifoBuf[256];
u8 cmdBuf[256];


char buf[32];

int main()
{
	int ch, value, re, len;
	char *str = "hello\r\n";
	sky_comDriver *uart1Dri;
	
	uart1Dri = uart_init(115200);
	
	P_StringCmdBindFifo(&scmd, uart1Dri, fifoBuf, sizeof(fifoBuf), cmdBuf, sizeof(cmdBuf));

	
	TIM1_PWM_Init(9999, 71);
	TIM_SetCompare1(TIM1,499);
	TIM_SetCompare4(TIM1,1499);
	
//	TIM_SetCompare1(TIM8, 499);	 //0.5ms
//	TIM_SetCompare2(TIM8, 2499); //2.5ms
//	TIM_SetCompare3(TIM8, 1499); //1.5ms
//	TIM_SetCompare4(TIM8, 4999); //5ms
	
	
	while (1)
	{
		if (P_StringCmdReadCMD(&scmd) > 0)
		{
			if (P_StringCmdScan(&scmd, "connection") >= 0)
			{
				len = sprintf(buf, "ok\n");
				uart1Dri->write(uart1Dri, (u8 *)buf, len);
			}
			else if (P_StringCmdScan(&scmd, "setmotorB%d:%d", &ch, &value) >= 0) //每秒脉冲数
			{
			}
		}
	}
//	while(1)
//	{
//		devWrite(uart1Dri, (u8 *)str, strlen(str));
//		rt_thread_mdelay(100);
//	}
}

/********************************* SKYCODE ************************************/
