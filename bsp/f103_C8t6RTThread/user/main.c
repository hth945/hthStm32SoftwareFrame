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


//开方
uint32_t axis_sqrt(uint32_t x)
{
  register uint32_t xr;  // result register
  register uint32_t q2;  // scan-bit register
  register uint8_t f;   // flag (one bit)

  xr = 0;                     // clear result
  q2 = 0x40000000L;           // higest possible result bit
  do
  {
    if((xr + q2) <= x)
    {
      x -= xr + q2;
      f = 1;                  // set flag
    }
    else{
      f = 0;                  // clear flag
    }
    xr >>= 1;
    if(f){
      xr += q2;               // test flag
    }
  } while(q2 >>= 2);          // shift twice
  if(xr < x){
    return xr +1;             // add for rounding
  }
  else{
    return xr;
	  }
}


void $Sub$$SystemInit (void)
{
	extern void $Super$$SystemInit (void);
	$Super$$SystemInit();
	SCB->VTOR = 0x08008000;
}


stringCMD_t scmd;
u8 fifoBuf[256];
u8 cmdBuf[256];

char buf[32];

const char cg_ver_message[] __attribute__((at(0X8008F00)))="hth+1.1.0+" __DATE__  "+" __TIME__ "+";

int main()
{
	int ch, value, re, len;
	char *str = "hello\r\n";
	sky_comDriver *uart1Dri;
	
	uart1Dri = uart1_init(115200);
	
	P_StringCmdBindFifo(&scmd, uart1Dri, fifoBuf, sizeof(fifoBuf), cmdBuf, sizeof(cmdBuf));

	
//	TIM1_PWM_Init(9999, 71);
//	TIM_SetCompare1(TIM1,499);
//	TIM_SetCompare4(TIM1,1499);
	
//	TIM_SetCompare1(TIM8, 499);	 //0.5ms
//	TIM_SetCompare2(TIM8, 2499); //2.5ms
//	TIM_SetCompare3(TIM8, 1499); //1.5ms
//	TIM_SetCompare4(TIM8, 4999); //5ms
	len = sprintf(buf, "hello\n");
	uart1Dri->write(uart1Dri, (u8 *)buf, len);
	
	TIM3_Int_Init(72, 0);
	while (1)
	{
		EncryRun();
		if (P_StringCmdReadCMD(&scmd) > 0)
		{
			if (P_StringCmdScan(&scmd, "connection") >= 0)
			{
				len = sprintf(buf, "ok2\n");
				uart1Dri->write(uart1Dri, (u8 *)buf, len);
			}else if (P_StringCmdScan(&scmd, "Rest") >= 0)
			{
//				len = sprintf(buf, "ok\n");
//				uart1Dri->write(uart1Dri, (u8 *)buf, len);
				rt_thread_mdelay(20);
				__set_FAULTMASK(1);
				NVIC_SystemReset();
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
