#include <string.h>
#include "systick.h"
#include "delay.h"
#include <stdio.h>
#include "bsp_usart1.h"	  
#include "bsp_usart5.h"	  
#include "Public_SoftStringCmd.h"
#include "communication.h"
#include <rtthread.h>
#include "mypwm.h"
#include "stmflash.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"	


void $Sub$$SystemInit (void)
{
	extern void $Super$$SystemInit (void);
	$Super$$SystemInit();
//	SCB->VTOR = 0x08008000;
}

stringCMD_t scmds[2];
u8 fifoBuf[128*2];
u8 cmdBuf[128*2];

char buf[32];

const char cg_ver_message[] __attribute__((at(0X8008F00)))="hth+1.1.0+" __DATE__  "+" __TIME__ "+";




int runStrCmd(stringCMD_t *scmd)
{
	int ch, value, re, len;
	if (P_StringCmdReadCMD(scmd) > 0)
	{
		if (P_StringCmdScan(scmd, "connection") >= 0)
		{
			len = sprintf(buf, "ok\n");
			devWrite(scmd->dev, (u8 *)buf, len);
		}else if (P_StringCmdScan(scmd, "Rest") >= 0)
		{
			rt_thread_mdelay(20);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		else if (P_StringCmdScan(scmd, "setmotorB%d:%d", &ch, &value) >= 0) //每秒脉冲数
		{
			
		}
	}
}

int main()
{
	int i = 0;
	sky_comDriver *uart1Dri;
	sky_comDriver *USBComDri;
	
	uart1Dri = uart1_init(115200);
	USBComDri = USBComInit();
	
	i = 0;
	P_StringCmdBindFifo(&scmds[i], uart1Dri, fifoBuf + i*128, 128, cmdBuf + i*128, 128);
	i = 1;
	P_StringCmdBindFifo(&scmds[i], USBComDri, fifoBuf + i*128, 128, cmdBuf + i*128, 128);
	
//	TIM1_PWM_Init(9999, 71);
//	TIM_SetCompare1(TIM1,499);
//	TIM_SetCompare4(TIM1,1499);
	
//	TIM_SetCompare1(TIM8, 499);	 //0.5ms
//	TIM_SetCompare2(TIM8, 2499); //2.5ms
//	TIM_SetCompare3(TIM8, 1499); //1.5ms
//	TIM_SetCompare4(TIM8, 4999); //5ms

	
	TIM3_Int_Init(72, 0);

	
//	
	while (1)
	{
//		EncryRun();
		for (i = 0; i < 2; i++)
		{
			runStrCmd(&scmds[i]);
		}
	}
//	while(1)
//	{
//		devWrite(uart1Dri, (u8 *)str, strlen(str));
//		rt_thread_mdelay(100);
//	}
}

/********************************* SKYCODE ************************************/
